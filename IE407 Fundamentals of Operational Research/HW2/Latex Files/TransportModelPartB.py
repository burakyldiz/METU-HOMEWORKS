import numpy as np
import pyomo.environ as pyo
from pyomo.opt import SolverFactory
import TransportData

# Data from the problem statement
# Import the exported dictionaries from TransportData
supplies = TransportData.exports['supplies']
costs_wh_dc = TransportData.exports['costs_wh_dc']
costs_dc_nh = TransportData.exports['costs_dc_nh']
demands = TransportData.exports['demands']
couriers = TransportData.exports['couriers']
warehouse_operating_costs = TransportData.exports['warehouse_operating_costs']
dc_operating_costs = TransportData.exports['dc_operating_costs']
moto_courier_salary = TransportData.exports['moto_courier_salary']
moto_courier_capacity = TransportData.exports['moto_courier_capacity']

# Construct the model
mdl = pyo.ConcreteModel('TransportModel')

# Define sets
mdl.W = pyo.Set(initialize=supplies.keys(), doc='Warehouses')
mdl.DC = pyo.Set(initialize=couriers.keys(), doc='Distribution Centers')
mdl.NH = pyo.Set(initialize=demands.keys(), doc='Neighborhoods')

# Define parameters
mdl.pS = pyo.Param(mdl.W, initialize=supplies, doc='Capacity of each warehouse')
mdl.pD = pyo.Param(mdl.NH, initialize=demands, doc='Demand of each neighborhood')
mdl.pC_wh_dc = pyo.Param(mdl.W, mdl.DC, initialize=costs_wh_dc, doc='Cost from warehouse to distribution center')
mdl.pC_dc_nh = pyo.Param(mdl.DC, mdl.NH, initialize=costs_dc_nh, doc='Cost from distribution center to neighborhood')
mdl.pCouriers = pyo.Param(mdl.DC, initialize=couriers, doc='Number of moto couriers')
mdl.pOperating_w = pyo.Param(mdl.W, initialize=warehouse_operating_costs, doc='Operating costs of warehouses')
mdl.pOperating_dc = pyo.Param(mdl.DC, initialize=dc_operating_costs, doc='Operating costs of distribution centers')

# Define variables
mdl.vX_wh_dc = pyo.Var(mdl.W, mdl.DC, within=pyo.NonNegativeReals, doc='Amount transported from warehouse to distribution center')
mdl.vX_dc_nh = pyo.Var(mdl.DC, mdl.NH, within=pyo.NonNegativeReals, doc='Amount transported from distribution center to neighborhood')

# Part B: Define continuous variables for operating warehouses and distribution centers (0 to 1)
mdl.yW = pyo.Var(mdl.W, within=pyo.Binary, doc='Warehouse operating decision')
mdl.yDC = pyo.Var(mdl.DC, within=pyo.Binary, doc='Distribution center operating decision')

# Define constraints

# Demand levels at neighborhoods
def eDemandLevel(mdl, nh):
    return sum(mdl.vX_dc_nh[dc, nh] for dc in mdl.DC) >= mdl.pD[nh]
mdl.eDemandLevel = pyo.Constraint(mdl.NH, rule=eDemandLevel, doc='Demand level constraint')

# Supply capacities at warehouses
def eSupplyCap(mdl, w):
    return sum(mdl.vX_wh_dc[w, dc] for dc in mdl.DC) <= mdl.pS[w] * mdl.yW[w]
mdl.eSupplyCap = pyo.Constraint(mdl.W, rule=eSupplyCap, doc='Supplier capacity constraint')

# Moto courier capacities at distribution centers
def eCourierCap(mdl, dc):
    return sum(mdl.vX_dc_nh[dc, nh] for nh in mdl.NH) <= mdl.pCouriers[dc] * moto_courier_capacity * mdl.yDC[dc]
mdl.eCourierCap = pyo.Constraint(mdl.DC, rule=eCourierCap, doc='Courier capacity constraint')

# Ensure flow conservation only for operating distribution centers
def eFlowConservation(mdl, dc):
    return sum(mdl.vX_wh_dc[w, dc] for w in mdl.W) == sum(mdl.vX_dc_nh[dc, nh] for nh in mdl.NH)
mdl.eFlowConservation = pyo.Constraint(mdl.DC, rule=eFlowConservation, doc='Flow conservation at operating distribution centers')

# Ensure flow conservation at operating warehouses
def eFlowConservationWH(mdl, w):
    return sum(mdl.vX_wh_dc[w, dc] for dc in mdl.DC) <= sum(mdl.vX_dc_nh[dc, nh] for dc in mdl.DC for nh in mdl.NH) + 1e6 * (1 - mdl.yW[w])
mdl.eFlowConservationWH = pyo.Constraint(mdl.W, rule=eFlowConservationWH, doc='Flow conservation at operating warehouses')

# Part B: Ensure that transport only happens if the facility is operational
def eTranspFromWhToDc(mdl, w, dc):
    return mdl.vX_wh_dc[w, dc] <= mdl.pS[w] * mdl.yW[w]
mdl.eTranspFromWhToDc = pyo.Constraint(mdl.W, mdl.DC, rule=eTranspFromWhToDc, doc='Transport from warehouse to DC only if operational')

def eTranspFromDcToNh(mdl, dc, nh):
    return mdl.vX_dc_nh[dc, nh] <= mdl.pCouriers[dc] * moto_courier_capacity * mdl.yDC[dc]
mdl.eTranspFromDcToNh = pyo.Constraint(mdl.DC, mdl.NH, rule=eTranspFromDcToNh, doc='Transport from DC to NH only if operational')

# Define objective function to find the minimum cost of serving neighborhoods, including fixed costs and salaries for part b.
def oTotal_Cost(mdl):
    # Transportation cost from warehouses to distribution centers
    transportation_cost = sum(mdl.pC_wh_dc[w, dc] * mdl.vX_wh_dc[w, dc] for w in mdl.W for dc in mdl.DC)
    
    # Delivery cost from distribution centers to neighborhoods
    delivery_cost = sum(mdl.pC_dc_nh[dc, nh] * mdl.vX_dc_nh[dc, nh] for dc in mdl.DC for nh in mdl.NH)
    
    # Operating costs for warehouses and distribution centers
    operating_cost = sum(mdl.pOperating_w[w] * mdl.yW[w] for w in mdl.W) + sum(mdl.pOperating_dc[dc] * mdl.yDC[dc] for dc in mdl.DC)
    
    # Part B: Include the salaries of moto couriers
    courier_salaries = sum(mdl.pCouriers[dc] * moto_courier_salary * mdl.yDC[dc] for dc in mdl.DC)
    
    return transportation_cost + delivery_cost + operating_cost + courier_salaries

mdl.oTotal_Cost = pyo.Objective(rule=oTotal_Cost, sense=pyo.minimize, doc='Total Transportation Cost')

# Export the open form of the model (optional)
mdl.write('mdl.lp', io_options={'symbolic_solver_labels': True})
mdl.dual = pyo.Suffix(direction=pyo.Suffix.IMPORT)  # shadow prices of the constraints
mdl.rc = pyo.Suffix(direction=pyo.Suffix.IMPORT)  # reduced costs of the objective function coefficients

# Solve the model
Solver = SolverFactory('glpk')

# Can not Print the sensitivity analysis and output report since this is an MIP

# The orders of constraints and variables are as in the Suffix Declarations
SolverResults = Solver.solve(mdl, tee=True)
SolverResults.write()

# Print the results
mdl.pprint()
mdl.vX_wh_dc.display()
mdl.vX_dc_nh.display()
mdl.oTotal_Cost.display()