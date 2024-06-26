from pyomo.environ import *

# Create a model
model = ConcreteModel()

# Define decision variables
model.x1 = Var(within=NonNegativeReals)  # x1 is continuous
model.x2 = Var(within=NonNegativeIntegers)  # x2 is integer
model.x3 = Var(within=NonNegativeIntegers)  # x3 is integer

# Define the objective function
model.obj = Objective(expr=8 * model.x1 + 6 * model.x2 + 2 * model.x3, sense=maximize)

# Define constraints
model.con1 = Constraint(expr=6 * model.x1 + 4 * model.x2 + 2 * model.x3 <= 14)
model.con2 = Constraint(expr=4 * model.x1 + 2 * model.x2 + 4 * model.x3 <= 22)

# Solve the model
solver = SolverFactory('glpk')
solver.solve(model, tee=True)

# Print results
print(f"x1: {model.x1()}")
print(f"x2: {model.x2()}")
print(f"x3: {model.x3()}")
print(f"Objective: {model.obj()}")
