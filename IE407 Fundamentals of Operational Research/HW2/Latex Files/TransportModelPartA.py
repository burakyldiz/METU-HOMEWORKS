import numpy as np
import TransportData
import pandas as pd
import pyomo.environ as pyo
from pyomo.opt import SolverFactory

# Import the exported dictionaries from TransportData
supplies = TransportData.exports['supplies']
costs_wh_dc = TransportData.exports['costs_wh_dc']
costs_dc_nh = TransportData.exports['costs_dc_nh']
demands = TransportData.exports['demands']
couriers = TransportData.exports['couriers']
moto_courier_capacity = TransportData.exports['moto_courier_capacity']

# Print the sets and dictionary keys for debugging
print("Warehouses:", supplies.keys())
print("Distribution Centers:", couriers.keys())
print("Neighborhoods:", demands.keys())
print("Costs WH to DC keys:", costs_wh_dc.keys())
print("Costs DC to NH keys:", costs_dc_nh.keys())
print("Moto Courier Capacity:", moto_courier_capacity)
print("Moto Couriers:", couriers)

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

# Define variables
mdl.vX_wh_dc = pyo.Var(mdl.W, mdl.DC, within=pyo.NonNegativeReals, doc='Amount transported from warehouse to distribution center')
mdl.vX_dc_nh = pyo.Var(mdl.DC, mdl.NH, within=pyo.NonNegativeReals, doc='Amount transported from distribution center to neighborhood')

# Define constraints
# Demand levels at neighborhoods
def eDemandLevel(mdl, nh):
    return sum(mdl.vX_dc_nh[dc, nh] for dc in mdl.DC) >= mdl.pD[nh]
mdl.eDemandLevel = pyo.Constraint(mdl.NH, rule=eDemandLevel, doc='Demand level constraint')

# Supply capacities at warehouses
def eSupplyCap(mdl, w):
    return sum(mdl.vX_wh_dc[w, dc] for dc in mdl.DC) <= mdl.pS[w]
mdl.eSupplyCap = pyo.Constraint(mdl.W, rule=eSupplyCap, doc='Supplier capacity constraint')

# Moto courier capacities at distribution centers
def eCourierCap(mdl, dc):
    return sum(mdl.vX_dc_nh[dc, nh] for nh in mdl.NH) <= mdl.pCouriers[dc] * moto_courier_capacity
mdl.eCourierCap = pyo.Constraint(mdl.DC, rule=eCourierCap, doc='Courier capacity constraint')

# Ensure flow conservation at distribution centers
def eFlowConservation(mdl, dc):
    return sum(mdl.vX_wh_dc[w, dc] for w in mdl.W) == sum(mdl.vX_dc_nh[dc, nh] for nh in mdl.NH)
mdl.eFlowConservation = pyo.Constraint(mdl.DC, rule=eFlowConservation, doc='Flow conservation at distribution centers')

# Define objective function to find the  minimum cost of serving neighborhoods.
def oTotal_Cost(mdl):
    transportation_cost = sum(mdl.pC_wh_dc[w, dc] * mdl.vX_wh_dc[w, dc] for w in mdl.W for dc in mdl.DC)
    delivery_cost = sum(mdl.pC_dc_nh[dc, nh] * mdl.vX_dc_nh[dc, nh] for dc in mdl.DC for nh in mdl.NH)
    
    return transportation_cost + delivery_cost
mdl.oTotal_Cost = pyo.Objective(rule=oTotal_Cost, sense=pyo.minimize, doc='Total Transportation Cost')

# Export the open form of the model (optional)
mdl.write('mdl.lp', io_options={'symbolic_solver_labels': True})
mdl.dual = pyo.Suffix(direction=pyo.Suffix.IMPORT)  # shadow prices of the constraints
mdl.rc = pyo.Suffix(direction=pyo.Suffix.IMPORT)  # reduced costs of the objective function coefficients

# Solve the model
Solver = SolverFactory('glpk')

# Print the sensitivity analysis and output report
Solver.options['ranges'] = '/Users/burak/Desktop/IE-HW2/Part-A/Output_PartA.txt'

# The orders of constraints and variables are as in the Suffix Declarations
SolverResults = Solver.solve(mdl, tee=True)
SolverResults.write()

# Print the results
mdl.pprint()
mdl.vX_wh_dc.display()
mdl.vX_dc_nh.display()
mdl.oTotal_Cost.display()


