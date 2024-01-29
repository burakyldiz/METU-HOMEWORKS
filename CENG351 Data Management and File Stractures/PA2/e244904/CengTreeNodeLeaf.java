import java.util.ArrayList;

public class CengTreeNodeLeaf extends CengTreeNode
{
    private ArrayList<CengBook> books;


    public CengTreeNodeLeaf(CengTreeNode parent)
    {
        super(parent);

        // Extra initializations
        this.type = CengNodeType.Leaf;
        this.books = new ArrayList<>();
    }

    // GUI Methods - Do not modify
    public int bookCount()
    {
        return books.size();
    }
    public Integer bookKeyAtIndex(Integer index)
    {
        if(index >= this.bookCount()) {
            return -1;
        } else {
            CengBook book = this.books.get(index);

            return book.getBookID();
        }
    }

    // Extra Functions
    public CengBook search(Integer bookID)
    {
        for (CengBook book : this.books) {
            if (book.getBookID().equals(bookID)) {
                return book;
            }
        }
        return null;
    }

    @Override
    public void addBook(CengBook book, CengTree tree)
    {
        boolean added = false;
        for (int i = 0; i < this.books.size(); i++)
        {
            CengBook index = this.books.get(i);

            if (index.getBookID() > book.getBookID())
            {
                this.books.add(i, book);
                added = true;
                break;
            }
        }

        if (added == false)
        {
            this.books.add(book);
        }

        if (this.books.size() > CengTreeNodeLeaf.order * 2)
        {
            int middle = (int) Math.floor((double) this.books.size() / 2);

            CengBook midBook = this.books.get(middle);

            CengTreeNodeLeaf newLeafNode = new CengTreeNodeLeaf(this.getParent());

            for (int i = middle; i < this.books.size(); i++)
            {
                newLeafNode.books.add(this.books.get(i));
            }

            if (this.books.size() > middle)
            {
                this.books.subList(middle, this.books.size()).clear();
            }

            if (this.getParent() == null)
            {
                CengTreeNodeInternal newRoot = new CengTreeNodeInternal(null);
                tree.root = newRoot;
                newRoot.addNode(this.books.get(0).getBookID(), this, tree);
                newRoot.addKey(midBook.getBookID());
                newRoot.addNode(midBook.getBookID(), newLeafNode, tree);
            }
            else
            {
                CengTreeNodeInternal parentNode = (CengTreeNodeInternal) this.getParent();
                parentNode.addKey(midBook.getBookID());
                parentNode.addNode(midBook.getBookID(), newLeafNode, tree);
            }
        }
    }

    @Override
    public void print(int indent)
    {
        this.printIndent(indent);
        System.out.println("<data>");

        for (CengBook book : this.books) {
            this.printBook(book, indent);
        }

        this.printIndent(indent);
        System.out.println("</data>");
    }

}
