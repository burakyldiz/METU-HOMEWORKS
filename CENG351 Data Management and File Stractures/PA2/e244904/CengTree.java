import java.util.ArrayList;

public class CengTree
{
    public CengTreeNode root;
    // Any extra attributes...

    public CengTree(Integer order)
    {
        CengTreeNode.order = order;
        // Initialize the class
        this.root = new CengTreeNodeLeaf(null);

    }

    public void addBook(CengBook book)
    {
        // Insert Book to Tree
        this.root.addBook(book, this);
    }

    public ArrayList<CengTreeNode> searchBook(Integer bookID)
    {
        // Search within whole Tree, return visited nodes.
        // Return null if not found.

        CengBook book = null;
        ArrayList<CengTreeNode> result = new ArrayList<>();
        CengTreeNode parent = this.root;

        while (parent != null)
        {
            result.add(parent);

            if (parent.type == CengNodeType.Internal)
            {
                CengTreeNodeInternal int_node = (CengTreeNodeInternal) parent;

                parent = int_node.search(bookID);
            }
            else if (parent.type == CengNodeType.Leaf)
            {
                CengTreeNodeLeaf nodeLeaf = (CengTreeNodeLeaf) parent;

                book = nodeLeaf.search(bookID);
                break;
            }
        }

        if (book == null)
        {
            System.out.print("Could not find ");
            System.out.print(bookID);
            System.out.println(".");

            return null;
        }

        for (int b = 0; b < result.size(); b++)
        {
            if (result.get(b).type == CengNodeType.Internal)
            {
                CengTreeNodeInternal nodeInternal = (CengTreeNodeInternal) result.get(b);

                nodeInternal.print_parent(b);
            }
            else if (result.get(b).type == CengNodeType.Leaf)
            {
                CengTreeNodeLeaf nodeLeaf = (CengTreeNodeLeaf) result.get(b);

                nodeLeaf.printBook(book, b);
            }
        }

        return result;
    }

    public void printTree()
    {
        //Print the whole tree to console
        if (this.root != null)
        {
            this.root.print(0);
        }
    }

    // Any extra functions...
}
