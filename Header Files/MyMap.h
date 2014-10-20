#ifndef SUBSTITUTEMYMAP_INCLUDED
#define SUBSTITUTEMYMAP_INCLUDED
#include <queue>
using namespace std;
template <class KeyType, class ValueType>
class MyMap
{
public:
    MyMap();

    ~MyMap();

    void clear();

    int size() const;
    void associate(const KeyType& key, const ValueType& value);

    const ValueType* find(const KeyType& key) const;
        
    ValueType* find(const KeyType& key);

    ValueType* getFirst(KeyType& key);

    ValueType* getNext(KeyType& key);

private:
    MyMap(const MyMap &other);
    MyMap &operator=(const MyMap &other);
	
    struct Node
	    {
			Node(){ right=0; left=0;}
		    KeyType treeKey;
	        ValueType treeValue;
	        Node* right;
		    Node* left;
	    };	
		
	
	class tree
	{
	private:
		
		
	public:
		void insert(Node*& ptr, KeyType keyNode, ValueType valNode);
		
		//Recursive function to delete each node in post order traversal and sets each deleted pointer to 0, so no bad access happens and so the root can be used again
		void deleteNodes(Node* ptr)
		{
		   if(ptr==0)
			   return;
		   deleteNodes(ptr->left);
		   deleteNodes(ptr->right);
		   delete ptr;
		   ptr=0;
		   numKeys--;
		}
		//Calls deleteNodes instead of duplicating code
		~tree()
		{
			deleteNodes(root);
		}
		//Initializes root and counter
		tree()
		{
			root=0;
		    numKeys=0;
		}
	Node* root;
	int numKeys;
	
	};
    queue<Node*> nodeQ;
	tree treeSeedling;
	
};

//Recursive function to insert new nodes or replace the ValueType data member if the key already exists.
template <class KeyType, class ValueType>
void MyMap<KeyType,ValueType>::tree::insert(Node*& ptr, KeyType keyNode, ValueType valNode)
{
	
    if(ptr==0)
	{
		ptr=new Node;
	    ptr->treeKey=keyNode;
	    ptr->treeValue=valNode;
		numKeys++;
	    return;
	}
	if(ptr->treeKey==keyNode)
	{
		ptr->treeValue=valNode;
		return;
	}
	if(keyNode>ptr->treeKey)
	   insert(ptr->right,keyNode, valNode);
	else
        insert(ptr->left,keyNode, valNode);
	

}

template <class KeyType, class ValueType>
MyMap<KeyType, ValueType>::MyMap()
{
}

//Calls clear to not duplicate code
template <class KeyType, class ValueType>
MyMap<KeyType,ValueType>::~MyMap()
{
	this->clear();
}

//calls deleteNodes and also sets the root to 0 so the tree can be used again
template <class KeyType, class ValueType>
void MyMap<KeyType, ValueType>::clear()
{
	this->treeSeedling.deleteNodes(treeSeedling.root);
	treeSeedling.root=0;
}

//Returns count of size of tree
template <class KeyType, class ValueType>
int MyMap< KeyType, ValueType>::size() const
{
	return treeSeedling.numKeys;  
}

//Calls tree's insert method
template <class KeyType, class ValueType>
void MyMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
	treeSeedling.insert(treeSeedling.root, key, value);
	
}

//Nonrecursive function that uses a while loop to find the node
template <class KeyType, class ValueType>
const ValueType* MyMap<KeyType, ValueType>::find(const KeyType& key) const
{
	Node* hold=treeSeedling.root;
	while(hold!=0)                          //If hold==NULL skip the while loop and return NULL
	{
		if(hold->treeKey==key)
			return &hold->treeValue;
		else
			if(hold->treeKey>key)           //Move to the left child if the key is greater than the current node's key
			   hold=hold->left;
			else
				hold=hold->right;           //Move to the right child if the key is less than the current node's key
	}
	return 0;
}
        
template <class KeyType, class ValueType>
 ValueType* MyMap<KeyType, ValueType>::find(const KeyType& key)
{
          // Do not change the implementation of this overload of find
        const MyMap<KeyType,ValueType>* constThis = this;
        return const_cast<ValueType*>(constThis->find(key));
}

//This returns the root node, the first value in the tree and pushes the children onto the queue for that level order traversal
template <class KeyType, class ValueType>
ValueType* MyMap< KeyType, ValueType>::getFirst(KeyType& key)
{
	if(treeSeedling.root!=0)              //If tree is not empty
	{
		if(treeSeedling.root->left!=0)
			nodeQ.push(treeSeedling.root->left);          //Push the left child, if not NULL, onto the queue
		if(treeSeedling.root->right!=0)  
			nodeQ.push(treeSeedling.root->right);         //Push the right child, if not NULL, onto the queue       
		key=treeSeedling.root->treeKey;                   //Set key
		return &treeSeedling.root->treeValue;             //return the value at the root                 
	}
	else 
		return 0;                                           //If tree is empty return NULL

}

//Utilizes a queue to print out nodes one at a time for each call to getNext in level order
 template <class KeyType, class ValueType>
 ValueType* MyMap< KeyType, ValueType>::getNext(KeyType& key)
{
	if(nodeQ.empty())                      //If the queue is empty, we have traversed through all the elements
		return 0;
	else
	{
		Node* peek=nodeQ.front();                //Get the front node in the queue
		nodeQ.pop();                             //Pop it out of the queue
		if(peek->left!=0)
			nodeQ.push(peek->left);               //If it has a left child, push it onto the queue
		if(peek->right!=0)
			nodeQ.push(peek->right);                 //If it has a right child push it ono the queue
		key=peek->treeKey;                            //Set key
     	return &peek->treeValue;                      //return the value in current node
	}
}

#endif // SUBSTITUTEMYMAP_INCLUDED