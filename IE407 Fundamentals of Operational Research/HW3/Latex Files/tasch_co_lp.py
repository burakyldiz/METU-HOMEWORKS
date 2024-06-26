from pyomo.environ import *

# Create a model
model = ConcreteModel()

# Define decision variables
model.x1 = Var(within=NonNegativeReals)  # Number of PCs produced
model.x2 = Var(within=NonNegativeReals)  # Number of Tablets produced
model.x3 = Var(within=NonNegativeReals)  # Number of Microprocessors produced

# Define the objective function
model.obj = Objective(expr=50 * model.x1 + 30 * model.x2 - 2 * model.x3, sense=maximize)

# Define constraints
model.con1 = Constraint(expr=5 * model.x1 + 2 * model.x2 + model.x3 <= 120)
model.con2 = Constraint(expr=8 * model.x1 + 4 * model.x2 <= 80 + model.x3)

# Solve the model
solver = SolverFactory('glpk')
solver.solve(model, tee=True)

# Print results
print(f"x1: {model.x1()}")
print(f"x2: {model.x2()}")
print(f"x3: {model.x3()}")
print(f"Objective: {model.obj()}")
