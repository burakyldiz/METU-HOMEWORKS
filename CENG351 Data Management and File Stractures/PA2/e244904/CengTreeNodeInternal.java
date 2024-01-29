import java.util.ArrayList;

public class CengTreeNodeInternal extends CengTreeNode
{
    private ArrayList<Integer> keys;
    private ArrayList<CengTreeNode> children;

    public CengTreeNodeInternal(CengTreeNode parent)
    {
        super(parent);

        // Extra initializations.
        this.type = CengNodeType.Internal;
        this.keys = new ArrayList<>();
        this.children = new ArrayList<>();
    }

    // GUI Methods - Do not modify
    public ArrayList<CengTreeNode> getAllChildren()
    {
        return this.children;
    }
    public Integer keyCount()
    {
        return this.keys.size();
    }
    public Integer keyAtIndex(Integer index)
    {
        if(index >= this.keyCount() || index < 0)
        {
            return -1;
        }
        else
        {
            return this.keys.get(index);
        }
    }

    // Extra Functions
    public void addKey(Integer key)
    {
        boolean added = false;
        for (int i = 0; i < this.keys.size(); i++)
        {
            if (this.keys.get(i) > key)
            {
                this.keys.add(i, key);
                added = true;
                break;
            }
        }

        if (!added)
        {
            this.keys.add(key);
        }
    }

    public void addNode(Integer key, CengTreeNode node, CengTree tree)
    {
        boolean added = false;
        for (int i = 0; i < this.keys.size(); i++)
        {
            if (this.keys.get(i) > key)
            {
                this.children.add(i, node);
                added = true;
                break;
            }
        }
        if (!added)
        {
            this.children.add(node);
        }

        node.setParent(this);

        if (this.keys.size() > CengTreeNode.order * 2)
        {
            int middle = (int) Math.floor((double) this.keys.size() / 2);
            int middle_key = this.keys.get(middle);
            CengTreeNodeInternal newIntNode = new CengTreeNodeInternal(this.getParent());

            for (int i = middle + 1; i < this.keys.size(); i++)
            {
                newIntNode.keys.add(this.keys.get(i));
            }

            if (this.keys.size() > middle)
            {
                this.keys.subList(middle, this.keys.size()).clear();
            }

            for (int i = middle + 1; i < this.children.size(); i++)
            {
                this.children.get(i).setParent(newIntNode);
                newIntNode.children.add(this.children.get(i));
            }

            if (this.children.size() > middle + 1)
            {
                this.children.subList(middle + 1, this.children.size()).clear();
            }

            if (this.getParent() == null)
            {
                CengTreeNodeInternal updated_root = new CengTreeNodeInternal(null);
                tree.root = updated_root;
                updated_root.addNode(this.keys.get(0), this, tree);
                updated_root.addKey(middle_key);
                updated_root.addNode(middle_key, newIntNode, tree);
            }
            else
            {
                CengTreeNodeInternal parentNode = (CengTreeNodeInternal) this.getParent();
                parentNode.addKey(middle_key);
                parentNode.addNode(middle_key, newIntNode, tree);
            }
        }
    }

    public CengTreeNode search(Integer bookID)
    {
        for (int i = 0; i < this.keys.size(); i++)
        {
            if (this.keys.get(i) > bookID)
            {
                return this.children.get(i);
            }
        }

        return this.children.get(this.children.size() - 1);
    }

    @Override
    public void addBook(CengBook book, CengTree tree)
    {
        boolean added = false;

        for (int i = 0; i < this.keys.size(); i++)
        {
            if (this.keys.get(i) > book.getBookID())
            {
                this.children.get(i).addBook(book, tree);
                added = true;
                break;
            }
        }

        if (!added)
        {
            this.children.get(this.children.size() - 1).addBook(book, tree);
        }
    }

    public void print_parent(int indent) {
        this.printIndent(indent);
        System.out.println("<index>");

        for (Integer key : this.keys) {
            this.printIndent(indent);
            System.out.println(key);
        }

        this.printIndent(indent);
        System.out.println("</index>");
    }

    @Override
    public void print(int indent) {
        this.print_parent(indent);

        indent++;

        for (CengTreeNode child : this.children) {
            child.print(indent);
        }
    }
}
