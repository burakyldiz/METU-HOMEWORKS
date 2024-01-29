package ceng.ceng351.cengfactorydb;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;


import ceng.ceng351.cengfactorydb.QueryResult.*;
public class CENGFACTORYDB implements ICENGFACTORYDB{

    private static final String user = "e2449049"; // userName
    private static final String password = "n1alIdl3ZZNQ"; // password
    private static final String host = "144.122.71.128"; // host name
    private static final String database = "db2449049"; // database name
    private static final int port = 8080; // port
    private Connection connection;
    @Override
    public void initialize() {
        String url = "jdbc:mysql://" + host + ":" + port + "/" + database;
        try
        {
            Class.forName("com.mysql.cj.jdbc.Driver");
            connection = DriverManager.getConnection(url, user, password);
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }


    @Override
    public int createTables()
    {
        int result = 0;
        try
        {
            Statement statement = connection.createStatement();
            // Factory(factoryId:int, factoryName:Text, factoryType:Text, country: Text)
            statement.executeUpdate("CREATE TABLE IF NOT EXISTS Factory("+
                    "factoryId INT PRIMARY KEY, " +
                    "factoryName TEXT, " +
                    "factoryType TEXT, " +
                    "country TEXT)");
            result++;
            //Employee(employeeId:int, employeeName:Text, department:Text, salary: int)
            statement.executeUpdate("CREATE TABLE IF NOT EXISTS Employee("+
                    "employeeId INT PRIMARY KEY," +
                    "employeeName TEXT, " +
                    "department TEXT, " +
                    "salary INT)");
            result ++;
            //Works In(factoryId:int, employeeId:int, startDate: Date)
            statement.executeUpdate("CREATE TABLE IF NOT EXISTS Works_In("+
                    "factoryId INT REFERENCES Factory(factoryId), " +
                    "employeeId INT REFERENCES Employee(employeeId) ON DELETE CASCADE , " +
                    "startDate DATE," +
                    "PRIMARY KEY (factoryId, employeeId)," +
                    "UNIQUE (factoryId, employeeId))");
            result ++;
            //Product(productId:int, productName: Text, productType: Text)
            statement.executeUpdate("CREATE TABLE IF NOT EXISTS Product("+
                    "productId INT PRIMARY KEY, " +
                    "productName TEXT, " +
                    "productType TEXT)");
            result ++;
            //Produce(factoryId:int, productId:int, amount: int, productionCost:int)
            statement.executeUpdate("CREATE TABLE IF NOT EXISTS Produce("+
                    "factoryId INT REFERENCES Factory(factoryId), " +
                    "productId INT REFERENCES  Product(productId), " +
                    "amount INT, " +
                    "productionCost INT,"+
                    "PRIMARY KEY (factoryId, productId))");
            result ++;
            //Shipment(factoryId:int, productId:int, amount: int, pricePerUnit:int)
            statement.executeUpdate("CREATE TABLE IF NOT EXISTS Shipment("+
                    "factoryId INT REFERENCES Factory(factoryId), " +
                    "productId INT REFERENCES  Product(productId), " +
                    "amount INT, " +
                    "pricePerUnit INT," +
                    "PRIMARY KEY (factoryId, productId))");
            result ++;
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        return result;
    }


    @Override
    public int dropTables()
    {
        int result = 0;
        try {
            Statement statement = connection.createStatement();
            statement.executeUpdate("SET FOREIGN_KEY_CHECKS = 0");
            statement.executeUpdate("DROP TABLE Factory");
            result++;
            statement.executeUpdate("DROP TABLE Employee");
            result++;
            statement.executeUpdate("DROP TABLE Works_In");
            result++;
            statement.executeUpdate("DROP TABLE Product");
            result++;
            statement.executeUpdate("DROP TABLE Produce");
            result++;
            statement.executeUpdate("DROP TABLE Shipment");
            result++;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }


    @Override
    public int insertFactory(Factory[] factories)
    {
        int result = 0;
        try {
            Statement statement = connection.createStatement();
            for (Factory factory : factories) {
                result += statement.executeUpdate("INSERT INTO Factory VALUES(" +
                        factory.getFactoryId() + ", '" +
                        factory.getFactoryName() + "', '" +
                        factory.getFactoryType() + "', '" +
                        factory.getCountry() + "')");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }
    @Override
    public int insertEmployee(Employee[] employees)
    {
        int result = 0;
        try {
            Statement statement = connection.createStatement();
            for (Employee employee : employees) {
                result += statement.executeUpdate("INSERT INTO Employee VALUES(" +
                        employee.getEmployeeId() + ", '" +
                        employee.getEmployeeName() + "', '" +
                        employee.getDepartment() + "', " +
                        employee.getSalary() + ")");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }

    @Override
    public int insertWorksIn(WorksIn[] worksIns)
    {
        int result = 0;
        try {
            Statement statement = connection.createStatement();
            for (WorksIn worksIn : worksIns) {
                result += statement.executeUpdate(
                        "INSERT INTO Works_In VALUES(" +
                        worksIn.getFactoryId() + ", " +
                        worksIn.getEmployeeId() + ", '" +
                        worksIn.getStartDate() + "') ");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }

    @Override
    public int insertProduct(Product[] products)
    {
        int result = 0;
        try {
            Statement statement = connection.createStatement();
            for (Product product : products) {
                result += statement.executeUpdate("INSERT INTO Product VALUES('" +
                        product.getProductId() + "', '" +
                        product.getProductName() + "', '" +
                        product.getProductType() + "') ");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }


    @Override
    public int insertProduce(Produce[] produces)
    {
        int result = 0;
        try {
            Statement statement = connection.createStatement();
            for (Produce produce : produces) {
                result += statement.executeUpdate("INSERT INTO Produce VALUES(" +
                        produce.getFactoryId() + ", " +
                        produce.getProductId() + ", " +
                        produce.getAmount() + ", " +
                        produce.getProductionCost() + ")");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }


    @Override
    public int insertShipment(Shipment[] shipments)
    {
        int result = 0;
        try {
            Statement statement = connection.createStatement();
            for (Shipment shipment : shipments) {
                result += statement.executeUpdate("INSERT INTO Shipment VALUES(" +
                        shipment.getFactoryId() + ", " +
                        shipment.getProductId() + ", " +
                        shipment.getAmount() + ", " +
                        shipment.getPricePerUnit() + ")");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }

    @Override
    public Factory[] getFactoriesForGivenCountry(String country)
    {
        List<Factory> factories = new ArrayList<Factory>();
        try {
            Statement statement = connection.createStatement();
            ResultSet resultSet = statement.executeQuery(
                    "SELECT factoryId, factoryName, factoryType, country FROM Factory WHERE country = '"
                            + country + "' ORDER BY factoryId ASC");
            while (resultSet.next()) {
                factories.add(new Factory(resultSet.getInt("factoryId"), resultSet.getString("factoryName"),
                        resultSet.getString("factoryType"), resultSet.getString("country")));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return factories.toArray(new Factory[factories.size()]);
    }



    @Override
    public Factory[] getFactoriesWithoutAnyEmployee()
    {
        List<Factory> factories = new ArrayList<Factory>();
        try {
            Statement statement = connection.createStatement();
            ResultSet resultSet = statement
                    .executeQuery(
                            "SELECT factoryId, factoryName, factoryType, country FROM Factory WHERE factoryId NOT IN (SELECT factoryId FROM Works_In) ORDER BY factoryId ASC");
            while (resultSet.next()) {
                factories.add(new Factory(resultSet.getInt("factoryId"), resultSet.getString("factoryName"),
                        resultSet.getString("factoryType"), resultSet.getString("country")));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return factories.toArray(new Factory[factories.size()]);
    }

   @Override
    public Factory[] getFactoriesProducingAllForGivenType(String productType)
   {
       List<Factory> factories = new ArrayList<Factory>();
       try {
           Statement statement = connection.createStatement();
           ResultSet resultSet = statement.executeQuery(
                   "SELECT F.factoryId, F.factoryName, F.factoryType, F.country " +
                   "FROM Factory F " +
                   "JOIN Produce P ON F.factoryId = P.factoryId " +
                   "JOIN Product PR ON P.productId = PR.productId " +
                   "WHERE PR.productType = '" + productType + "' " +
                   "GROUP BY F.factoryId, F.factoryName, F.factoryType, F.country " +
                   "HAVING COUNT(DISTINCT P.productId) = ( " +
                   "    SELECT COUNT(productId) " +
                   "    FROM Product " +
                   "    WHERE productType = '" + productType + "' " +
                   ") " +
                   "ORDER BY F.factoryId ASC"
);
           while (resultSet.next()) {
               factories.add(new Factory(resultSet.getInt("factoryId"), resultSet.getString("factoryName"),
                       resultSet.getString("factoryType"), resultSet.getString("country")));
           }
       } catch (Exception e) {
           e.printStackTrace();
       }
       return factories.toArray(new Factory[factories.size()]);
   }

    @Override
    public Product[] getProductsProducedNotShipped()
    {
        List<Product> products = new ArrayList<Product>();
        try {
            Statement statement = connection.createStatement();
            ResultSet resultSet = statement.executeQuery("SELECT p.productId, p.productName, p.productType " +
                    "FROM Product p " +
                    "JOIN Produce pr ON p.productId = pr.productId " +
                    "WHERE NOT EXISTS (" +
                    "    SELECT 1 " +
                    "    FROM Shipment s " +
                    "    WHERE s.productId = pr.productId AND s.factoryId = pr.factoryId" +
                    ") " +
                    "GROUP BY p.productId, p.productName, p.productType " +
                    "ORDER BY p.productId ASC"
            );

            while (resultSet.next()) {
                products.add(new Product(resultSet.getInt("productId"), resultSet.getString("productName"),
                        resultSet.getString("productType")));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return products.toArray(new Product[products.size()]);
    }


    @Override
    public double getAverageSalaryForFactoryDepartment(int factoryId, String department) {
        double averageSalary=0;
        try
        {
            Statement statement = connection.createStatement();
            ResultSet resultSet = statement.executeQuery(
                    "SELECT AVG(salary) FROM Employee WHERE department = '" +
                            department +
                            "' AND employeeId IN(SELECT employeeId FROM Works_In WHERE factoryId  = '" +
                            factoryId +
                            "')");
            resultSet.next();
            averageSalary = resultSet.getDouble(1);
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }

        return averageSalary;
    }

    @Override
    public QueryResult.MostValueableProduct[] getMostValueableProducts()
    {
        List<MostValueableProduct> mvp = new ArrayList<MostValueableProduct>();
        double profit;
        try {
            Statement statement = connection.createStatement();
            ResultSet resultSet = statement.executeQuery("WITH ProfitCalc AS (\n" +
                    "    SELECT F.factoryId, P.productId, P.productName, P.productType,\n" +
                    "           CASE \n" +
                    "               WHEN S.productId IS NULL THEN -R.productionCost * R.amount\n" +
                    "               ELSE (S.pricePerUnit * S.amount - R.productionCost * R.amount)\n" +
                    "           END AS Profit,\n" +
                    "           ROW_NUMBER() OVER (\n" +
                    "               PARTITION BY F.factoryId \n" +
                    "               ORDER BY \n" +
                    "                   CASE \n" +
                    "                       WHEN S.productId IS NULL THEN -R.productionCost * R.amount\n" +
                    "                       ELSE (S.pricePerUnit * S.amount - R.productionCost * R.amount)\n" +
                    "                   END DESC\n" +
                    "           ) as rn\n" +
                    "    FROM Factory F\n" +
                    "    JOIN Produce R ON F.factoryId = R.factoryId\n" +
                    "    JOIN Product P ON R.productId = P.productId\n" +
                    "    LEFT JOIN Shipment S ON P.productId = S.productId AND R.factoryId = S.factoryId\n" +
                    ")\n" +
                    "SELECT factoryId, productId, productName, productType, Profit\n" +
                    "FROM ProfitCalc\n" +
                    "WHERE rn = 1\n" +
                    "ORDER BY Profit DESC, productId ASC");
            while (resultSet.next()) {
                mvp.add(new MostValueableProduct(resultSet.getInt("factoryId"), resultSet.getInt("productID"), resultSet.getString("productName"),
                        resultSet.getString("productType"),resultSet.getDouble("profit")));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return mvp.toArray(new MostValueableProduct[mvp.size()]);
    }


    @Override
    public QueryResult.MostValueableProduct[] getMostValueableProductsOnFactory() {
        List<MostValueableProduct> mvp = new ArrayList<MostValueableProduct>();

        try {
            Statement statement = connection.createStatement();
            ResultSet resultSet = statement.executeQuery("WITH ProfitCalc AS (\n" +
                    "    SELECT P.productId, F.factoryId,\n" +
                    "           CASE\n" +
                    "               WHEN S.productId IS NULL THEN -R.productionCost * R.amount\n" +
                    "               ELSE (S.pricePerUnit * S.amount - R.productionCost * R.amount)\n" +
                    "           END AS Profit\n" +
                    "    FROM Product P\n" +
                    "    JOIN Produce R ON P.productId = R.productId\n" +
                    "    JOIN Factory F ON R.factoryId = F.factoryId\n" +
                    "    LEFT JOIN Shipment S ON R.productId = S.productId AND R.factoryId = S.factoryId\n" +
                    "),\n" +
                    "MaxProfit AS (\n" +
                    "    SELECT productId, MAX(Profit) AS maxProfit\n" +
                    "    FROM ProfitCalc\n" +
                    "    GROUP BY productId\n" +
                    ")\n" +
                    "SELECT P.productId, P.productName, P.productType, PC.factoryId, PC.Profit\n" +
                    "FROM Product P\n" +
                    "JOIN ProfitCalc PC ON P.productId = PC.productId\n" +
                    "JOIN MaxProfit MP ON PC.productId = MP.productId AND PC.Profit = MP.maxProfit\n" +
                    "ORDER BY PC.Profit DESC, P.productId ASC");
            while (resultSet.next()) {
                mvp.add(new MostValueableProduct(resultSet.getInt("factoryId"), resultSet.getInt("productID"), resultSet.getString("productName"),
                        resultSet.getString("productType"),resultSet.getDouble("profit")));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return mvp.toArray(new MostValueableProduct[mvp.size()]);
    }


    @Override
    public QueryResult.LowSalaryEmployees[] getLowSalaryEmployeesForDepartments() {
        List<LowSalaryEmployees> lse = new ArrayList<LowSalaryEmployees>();
        try {
            Statement statement = connection.createStatement();
            ResultSet resultSet = statement.executeQuery( "SELECT DISTINCT E.employeeId, E.employeeName, E.department, \n" +
                    "       CASE \n" +
                    "           WHEN WI.employeeId IS NULL THEN 0 \n" +
                    "           ELSE E.salary \n" +
                    "       END AS salary\n" +
                    "FROM Employee E\n" +
                    "LEFT JOIN Works_In WI ON E.employeeId = WI.employeeId\n" +
                    "WHERE \n" +
                    "    (CASE \n" +
                    "        WHEN WI.employeeId IS NULL THEN 0 \n" +
                    "        ELSE E.salary \n" +
                    "    END) < (\n" +
                    "        SELECT AVG(CASE \n" +
                    "                      WHEN WI2.employeeId IS NULL THEN 0 \n" +
                    "                      ELSE E2.salary \n" +
                    "                   END)\n" +
                    "        FROM Employee E2\n" +
                    "        LEFT JOIN Works_In WI2 ON E2.employeeId = WI2.employeeId\n" +
                    "        WHERE E2.department = E.department\n" +
                    "    )\n" +
                    "ORDER BY E.employeeId ASC");
            while (resultSet.next()) {
                lse.add(new LowSalaryEmployees(resultSet.getInt("employeeId"), resultSet.getString("employeeName"), resultSet.getString("department"),
                        resultSet.getInt("salary")));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return lse.toArray(new LowSalaryEmployees[lse.size()]);
    }


   @Override
    public int increaseCost(String productType, double percentage) {
        int rowsAffected = 0;
        try {
            Statement statement = connection.createStatement();
            rowsAffected = statement.executeUpdate("UPDATE Produce P " +
                    "JOIN Product PR ON P.productId = PR.productId " +
                    "SET P.productionCost = P.productionCost * (1 + " + percentage + ") " +
                    "WHERE PR.productType = '" + productType + "'");

        } catch (Exception e) {
            e.printStackTrace();
        }

        return rowsAffected;
    }


    @Override
    public int deleteNotWorkingEmployees(String givenDate) {
        int rowsAffected = 0;
        try {
            Statement statement = connection.createStatement();
            rowsAffected = statement.executeUpdate(
                    "DELETE " +
                    "FROM Employee " +
                    "WHERE employeeId NOT IN (SELECT employeeId FROM Works_In WHERE startDate >'" + givenDate + "')");

        } catch (Exception e) {
            e.printStackTrace();
        }

        return rowsAffected;
    }


    /**
     * *****************************************************
     * *****************************************************
     * *****************************************************
     * *****************************************************
     *  THE METHODS AFTER THIS LINE WILL NOT BE GRADED.
     *  YOU DON'T HAVE TO SOLVE THEM, LEAVE THEM AS IS IF YOU WANT.
     *  IF YOU HAVE ANY QUESTIONS, REACH ME VIA EMAIL.
     * *****************************************************
     * *****************************************************
     * *****************************************************
     * *****************************************************
     */

    /**
     * For each department, find the rank of the employees in terms of
     * salary. Peers are considered tied and receive the same rank. After
     * that, there should be a gap.
     *
     * @return QueryResult.EmployeeRank[]
     */
    public QueryResult.EmployeeRank[] calculateRank() {
        return new QueryResult.EmployeeRank[0];
    }

    /**
     * For each department, find the rank of the employees in terms of
     * salary. Everything is the same but after ties, there should be no
     * gap.
     *
     * @return QueryResult.EmployeeRank[]
     */
    public QueryResult.EmployeeRank[] calculateRank2() {
        return new QueryResult.EmployeeRank[0];
    }

    /**
     * For each factory, calculate the most profitable 4th product.
     *
     * @return QueryResult.FactoryProfit
     */
    public QueryResult.FactoryProfit calculateFourth() {
        return new QueryResult.FactoryProfit(0,0,0);
    }

    /**
     * Determine the salary variance between an employee and another
     * one who began working immediately after the first employee (by
     * startDate), for all employees.
     *
     * @return QueryResult.SalaryVariant[]
     */
    public QueryResult.SalaryVariant[] calculateVariance() {
        return new QueryResult.SalaryVariant[0];
    }

    /**
     * Create a method that is called once and whenever a Product starts
     * losing money, deletes it from Produce table
     *
     * @return void
     */
    public void deleteLosing() {

    }
}
