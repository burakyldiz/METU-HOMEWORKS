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
moto_courier_salary = TransportData.exports['moto_courier_salary']
warehouse_operating_costs = TransportData.exports['warehouse_operating_costs']
dc_operating_costs = TransportData.exports['dc_operating_costs']

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

# Additional variables for part C
mdl.vX_dc1_nh_below = pyo.Var(mdl.NH, within=pyo.NonNegativeReals, doc='Amount transported from DC1 to neighborhoods below threshold')
mdl.vX_dc1_nh_above = pyo.Var(mdl.NH, within=pyo.NonNegativeReals, doc='Amount transported from DC1 to neighborhoods above threshold')

# Part B: Define binary variables for operating warehouses and distribution centers (0 or 1)
mdl.yW = pyo.Var(mdl.W, within=pyo.Binary, doc='Warehouse operating decision')
mdl.yDC = pyo.Var(mdl.DC, within=pyo.Binary, doc='Distribution center operating decision')

# Define constraints
threshold_deliveries = 2500
original_cost_per_km = 2.5
reduced_cost_per_km = 1.5

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

# Splitting the deliveries for DC1 into two parts
def eSplitDeliveries(mdl, nh):
    return mdl.vX_dc_nh[1, nh] == mdl.vX_dc1_nh_below[nh] + mdl.vX_dc1_nh_above[nh]
mdl.eSplitDeliveries = pyo.Constraint(mdl.NH, rule=eSplitDeliveries, doc='Splitting deliveries for DC1')

# Ensuring the correct costs for the split deliveries
def eThresholdCostBelow(mdl, nh):
    return mdl.vX_dc1_nh_below[nh] <= threshold_deliveries
mdl.eThresholdCostBelow = pyo.Constraint(mdl.NH, rule=eThresholdCostBelow, doc='Cost for deliveries below threshold')

def eThresholdCostAbove(mdl, nh):
    return mdl.vX_dc1_nh_above[nh] >= mdl.vX_dc_nh[1, nh] - threshold_deliveries
mdl.eThresholdCostAbove = pyo.Constraint(mdl.NH, rule=eThresholdCostAbove, doc='Cost for deliveries above threshold')

# Define objective function to find the minimum cost of serving neighborhoods
def oTotal_Cost(mdl):
    transportation_cost = sum(mdl.pC_wh_dc[w, dc] * mdl.vX_wh_dc[w, dc] for w in mdl.W for dc in mdl.DC)
    delivery_cost = sum(mdl.pC_dc_nh[dc, nh] * mdl.vX_dc_nh[dc, nh] for dc in mdl.DC for nh in mdl.NH if dc != 'Distribution Center 1')
    
    # Delivery costs for DC1 split into two parts
    delivery_cost += sum(original_cost_per_km * mdl.vX_dc1_nh_below[nh] for nh in mdl.NH)
    delivery_cost += sum(reduced_cost_per_km * mdl.vX_dc1_nh_above[nh] for nh in mdl.NH)
    
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
solver = SolverFactory('glpk')

# Cannot print the sensitivity analysis and output report since this is an MIP

# The orders of constraints and variables are as in the Suffix Declarations
solver_results = solver.solve(mdl, tee=True)
solver_results.write()

# Print the results
mdl.pprint()
mdl.vX_wh_dc.display()
mdl.vX_dc_nh.display()
mdl.oTotal_Cost.display()
