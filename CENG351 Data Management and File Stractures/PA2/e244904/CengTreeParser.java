import java.io.*;
import java.util.*;

public class CengTreeParser
{
    public static ArrayList<CengBook> parseBooksFromFile(String filename)
    {
        ArrayList<CengBook> bookList = new ArrayList<CengBook>();

        // You need to parse the input file in order to use GUI tables.
        // Parse the input file, and convert them into CengBooks
        try
        {
            File text = new File(filename);
            Scanner scan = new Scanner(text);

            while(scan.hasNextLine())
            {
                String line = scan.nextLine();
                String[] variables = line.split("\\|");

                bookList.add(new CengBook(
                        Integer.parseInt(variables[0]),
                        variables[1],
                        variables[2],
                        variables[3]
                ));
            }
        }
        catch (FileNotFoundException e)
        {
            e.printStackTrace();
        }

        return bookList;
    }

    public static void startParsingCommandLine() throws IOException
    {
        // Start listening and parsing command line -System.in-.

        Scanner scan = new Scanner(System.in);

        label:
        while (scan.hasNextLine()) {
            String line = scan.nextLine();

            String[] variables = line.split("\\|");

            String command = variables[0];

            switch (command) {
                case "quit":
                    break;
                case "add":
                    CengBook cengBook = new CengBook(
                            Integer.parseInt(variables[1]),
                            variables[2],
                            variables[3],
                            variables[4]
                    );

                    CengBookRunner.addBook(cengBook);
                    break;
                case "search":
                    CengBookRunner.searchBook(Integer.parseInt(variables[1]));
                    break;
                case "print":
                    CengBookRunner.printTree();
                    break;
            }
        }
    }
}
