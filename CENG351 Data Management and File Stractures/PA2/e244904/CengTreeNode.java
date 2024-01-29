import java.awt.Color;

public abstract class CengTreeNode
{
    static protected Integer order;
    private CengTreeNode parent;
    protected CengNodeType type; // Type needs to be set for proper GUI. Check CengNodeType.java.

    // GUI Accessors - do not modify
    public Integer level;
    public Color color;

    // Any extra attributes, if necessary

    public CengTreeNode(CengTreeNode parent)
    {
        this.parent = parent;

        this.color = CengGUI.getRandomBorderColor();

        // Extra initializations.
    }

    // Getters-Setters - Do not modify
    public CengTreeNode getParent()
    {
        return parent;
    }

    public void setParent(CengTreeNode parent)
    {
        this.parent = parent;
    }

    public CengNodeType getType()
    {
        return type;
    }

    // GUI Methods - Do not modify
    public Color getColor()
    {
        return this.color;
    }

    // Extra Functions

    public final String indent_creator(int indent) {
        String s = "";
        for (int i = 0; i < indent; i++) {
            s += "\t";
        }
        return s;
    }

    public final void printIndent(int indent) {
        System.out.print(this.indent_creator(indent));
    }

    public abstract void addBook(CengBook book, CengTree tree);

    public abstract void print(int indent);

    public final void printBook(CengBook book, int indent) {
        this.printIndent(indent);

        System.out.println("<record>" + book.getBookID() + "|" + book.getBookTitle() + "|" + book.getAuthor() + "|" + book.getGenre() + "</record>");
    }
}
