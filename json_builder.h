/*


*/
#include "json.h"
#include <vector>

namespace json {
    
    
class DictContext;    
class ArrayContext;   
class KeyContext;   
    

class Builder{
public:

    DictContext StartDict();

    ArrayContext StartArray();

    Builder& Value(json::Node value);

    Node Build();

    Builder& EndDict();

    Builder& EndArray();

    Builder& Key(std::string s);

private:
    json::Node* value_ptr_ = nullptr;
    json::Node root_;
    std::vector<json::Node*> nodes_stack_; 

};
   

class DictContext { 
public: 
    DictContext(Builder& builder) : builder_(builder){}

    KeyContext Key(std::string s);

    Builder EndDict();

private:
    Builder& builder_;

};    


class ArrayContext {
friend Builder;

public:
    ArrayContext(Builder& builder) : builder_(builder){}

    ArrayContext Value(json::Node value);

    Builder& EndArray();

    DictContext StartDict();

    ArrayContext StartArray();

private:
    Builder& builder_;
};  


class KeyContext {
public:
    KeyContext(Builder& builder) : builder_(builder){}

    DictContext Value(json::Node value);

    ArrayContext StartArray();

    DictContext StartDict();

private:
    Builder& builder_;

};  
    
}