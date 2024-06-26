import numpy as np
import pandas as pd

# Load the data from the Excel file
file_path = '/Users/burak/Desktop/IE-HW2/Part-A/TransportationData.xlsx'

# Capacities
capacities = pd.read_excel(file_path, sheet_name='Capacities')
capacities = capacities.set_index('Warehouse').to_dict()['Capacity']

# Distances from Warehouses to Distribution Centers
distances_wh_dc = pd.read_excel(file_path, sheet_name='Distances_WH_DC')
distances_wh_dc = distances_wh_dc.set_index('Warehouse').to_dict()

# Distances from Distribution Centers to Neighborhoods
distances_dc_nh = pd.read_excel(file_path, sheet_name='Distances_DC_NH')
distances_dc_nh = distances_dc_nh.set_index('Distribution Center').to_dict()

# Number of Moto Couriers
moto_couriers = pd.read_excel(file_path, sheet_name='MotoCouriers')
moto_couriers = moto_couriers.set_index('Distribution Center').to_dict()['Moto Couriers']

# Monthly Demand
demand = pd.read_excel(file_path, sheet_name='Demand')
demand = demand.set_index('Neighborhood').to_dict()['Monthly Demand']

# Operating Costs
operating_costs = pd.read_excel(file_path, sheet_name='OperatingCosts')
operating_costs = operating_costs.set_index('Location').to_dict()['Operating Cost']

# Constants
cost_per_km_wh_dc = 0.03  # Cost per km from warehouses to distribution centers
cost_per_km_dc_nh = 2.5  # Cost per km from distribution centers to neighborhoods
moto_courier_salary = 2350  # Monthly salary of a moto courier
moto_courier_capacity = 450  # Deliveries per moto courier per month

# Create dictionaries for parameters
supplies = {}
costs_wh_dc = {}
costs_dc_nh = {}
demands = {}
couriers = {}
operating = {}

# Fill supplies dictionary
for wh, capacity in capacities.items():
    supplies[wh] = capacity

# Fill costs_wh_dc dictionary with corrected indices (warehouse, distribution center)
for wh, row in distances_wh_dc.items():
    for dc, distance in row.items():
        costs_wh_dc[(wh, dc)] = distance * cost_per_km_wh_dc

# Fill costs_dc_nh dictionary with corrected indices (distribution center, neighborhood)
for dc, row in distances_dc_nh.items():
    for nh, distance in row.items():
        costs_dc_nh[(dc, nh)] = distance * cost_per_km_dc_nh

# Fill demands dictionary
for nh, dem in demand.items():
    demands[nh] = dem

# Fill couriers dictionary
for dc, num in moto_couriers.items():
    couriers[dc] = num

# Fill operating dictionary
for loc, cost in operating_costs.items():
    operating[loc] = cost

print('Supplies:', supplies)
print('Costs WH to DC:', costs_wh_dc)
print('Costs DC to NH:', costs_dc_nh)
print('Demands:', demands)
print('Couriers:', couriers)
print('Operating Costs:', operating)

# Export dictionaries
exports = {
    'supplies': supplies,
    'costs_wh_dc': costs_wh_dc,
    'costs_dc_nh': costs_dc_nh,
    'demands': demands,
    'couriers': couriers,
    'operating': operating,
    'cost_per_km_wh_dc': cost_per_km_wh_dc,
    'cost_per_km_dc_nh': cost_per_km_dc_nh,
    'moto_courier_salary': moto_courier_salary,
    'moto_courier_capacity': moto_courier_capacity
}