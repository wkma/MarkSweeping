#include<iostream>
#include<vector>
#include<unordered_set>
#include<algorithm>


using namespace std;

class Object;

class GC{
public:
    vector<Object*> objects;
    unordered_set<Object*> roots;

    void addRoot(Object* obj)
    {
        roots.insert(obj);
    }

    void removeRoot(Object* obj)
    {
        roots.erase(obj);
    }

    void collect();
};

GC& getGC()
{
    static GC gc;
    return gc;
}

class Object
{
public:
    bool marked;  //标记
    vector<Object*> references;

    Object() :marked(false){
        getGC().objects.push_back(this);
    }

    void adddReference(Object* obj)
    {
        references.push_back(obj);
    }

    virtual void print() const=0;
    virtual ~Object() {}
};

class Node:public Object {
public:
    string name;
    Node(const string& name):name(name)
    {

    }
    void print() const override{
        cout<<"Node :"<<name <<endl;
    }
};

void GC::collect(){
    cout<<"begin gc"<<endl;
    vector<Object*> stack(roots.begin(),roots.end());

    size_t index = 0;

    while(index<stack.size())
    {
        Object* obj = stack[index++];

        if(!obj->marked){
            obj->marked =true;
            for(auto& ref:obj->references){
                if(ref&&!ref->marked){
                    stack.push_back(ref);
                }
            }
        }
    }
    std::vector<Object*>::iterator it = objects.begin();
    while(it!=objects.end())
    {
        Object *obj =*it;
        if(!obj->marked)
        {
            cout<<"Freed:";
            obj->print();
            cout<< endl;
            delete obj;
            it =objects.erase(it);
        }else{
            obj->marked =false;
            ++it;
        }
    }
    cout<< "end gc"<<endl;
}

Node* newNode(const string& name){
    return new Node(name);
}
int main(){
    GC& gc =getGC();
    Node* nodeA = newNode("A");
    Node* nodeB = newNode("B");
    Node* nodeC = newNode("C");
    Node* nodeD = newNode("D");

    nodeA->adddReference(nodeB);
    nodeB->adddReference(nodeC);
    nodeC->adddReference(nodeA); //循环引用
    nodeD->adddReference(nodeB);

    gc.addRoot(nodeA);
    gc.addRoot(nodeD);

    gc.collect();

    gc.removeRoot(nodeA);
    gc.collect();
    gc.removeRoot(nodeD);  
    gc.collect();
    return 0;
}
