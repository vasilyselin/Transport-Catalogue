#include "json_builder.h"

namespace json {

    KeyContext DictContext::Key(std::string s) {
        builder_.Key(s);
        return builder_;
    }    

    DictContext Builder::StartDict(){
        if(nodes_stack_.size() != 0 && nodes_stack_.back()->IsDict() && value_ptr_ != nullptr){
            nodes_stack_.push_back(new json::Node(json::Dict()));
            *value_ptr_ = *(nodes_stack_.back());
            value_ptr_ = nullptr;
        }
        else if(nodes_stack_.size() != 0 && nodes_stack_.back()->IsArray()){
            const_cast<json::Array&>(nodes_stack_.back()->AsArray()).push_back(json::Dict());
            nodes_stack_.push_back(&(const_cast<json::Array&>(nodes_stack_.back()->AsArray()).back()));
        }
        else if(nodes_stack_.size() == 0 && std::holds_alternative<std::nullptr_t>(root_.GetValue())){
            nodes_stack_.push_back(new json::Node(json::Dict()));
        }
        else{
            throw std::logic_error("method \"StartDict\" is called outside of Dict contex");
        }
        return *this;
    }


    ArrayContext Builder::StartArray(){
        if(nodes_stack_.size() != 0 && nodes_stack_.back()->IsDict() && value_ptr_ != nullptr){
            *value_ptr_ = json::Array();
            nodes_stack_.push_back(value_ptr_);
            value_ptr_ = nullptr;            
        }
        else if(nodes_stack_.size() != 0 && nodes_stack_.back()->IsArray()){
            const_cast<json::Array&>(nodes_stack_.back()->AsArray()).emplace_back(json::Array());
            nodes_stack_.push_back(&(const_cast<json::Array&>(nodes_stack_.back()->AsArray()).back()));            
        }
        else if(nodes_stack_.size() == 0 && std::holds_alternative<std::nullptr_t>(root_.GetValue())){          
            nodes_stack_.push_back(new json::Node(json::Array()));
        }
        else{
            throw std::logic_error("method \"StartArray\" is called outside of contex");
        }
        return *this;
    } 

    Builder& Builder::Value(json::Node value){
        if(nodes_stack_.size() != 0 && nodes_stack_.back()->IsDict() && value_ptr_ != nullptr){
            *value_ptr_ = move(value);
            value_ptr_ = nullptr;
        }
        else if(nodes_stack_.size() != 0 && nodes_stack_.back()->IsArray()){
            const_cast<json::Array&>(nodes_stack_.back()->AsArray()).push_back(move(value));
        }
        else if(nodes_stack_.size() == 0 && std::holds_alternative<std::nullptr_t>(root_.GetValue())){
            root_ = move(value);
        }
        else{
            throw std::logic_error("method \"Value\" is called outside of contex");
        }
        return *this;
    }       
    
    Node Builder::Build(){
        if(!std::holds_alternative<std::nullptr_t>(root_.GetValue()) && value_ptr_ == nullptr && nodes_stack_.size() == 0){
            return root_;
        }
        else{
            throw std::logic_error("method \"Build\" is called outside of contex");
        }
        return root_;        
    }

    Builder& Builder::EndDict(){
        if(nodes_stack_.size() != 0 && nodes_stack_.back()->IsDict() && value_ptr_ == nullptr){
            if(nodes_stack_.size() == 1 && std::holds_alternative<std::nullptr_t>(root_.GetValue())){
                root_ = move(*nodes_stack_.back());
            }            
            nodes_stack_.erase(nodes_stack_.end()-1);
        }
        else{
            throw std::logic_error("method \"EndDict\" is called outside of Dict contex");
        }
        return *this;
    }

    Builder& Builder::EndArray(){
        if(nodes_stack_.size() != 0 && nodes_stack_.back()->IsArray()){
            if(nodes_stack_.size() == 1 && std::holds_alternative<std::nullptr_t>(root_.GetValue())){
                root_ = move(*nodes_stack_.back());
            }
            nodes_stack_.erase(nodes_stack_.end()-1);
        }
        else{
            throw std::logic_error("method \"EndArray\" is called outside of Dict contex");
        }
        return *this;
    }

    Builder& Builder::Key(std::string s){
        if(nodes_stack_.size() != 0 && nodes_stack_.back()->IsDict() && value_ptr_ == nullptr){
            auto it = const_cast<json::Dict&>(nodes_stack_.back()->AsDict()).insert({move(s), json::Node()});
            value_ptr_ = &(it.first->second);
        }
        else{
            throw std::logic_error("method \"Key\" is called outside of Dict contex");
        }
        return *this;
    }

    Builder DictContext::EndDict(){
        builder_.EndDict();
        return builder_;
    }
    
    ArrayContext ArrayContext::Value(json::Node value){
        builder_.Value(value);
        return builder_;
    }

    Builder& ArrayContext::EndArray(){
        builder_.EndArray();
        return builder_;
    }

    DictContext ArrayContext::StartDict() {
        builder_.StartDict();
        return builder_;
    } 

    ArrayContext ArrayContext::StartArray(){
        builder_.StartArray();
        return builder_;
    } 

    DictContext KeyContext::Value(json::Node value){
        builder_.Value(value);
        return builder_;
    }

    ArrayContext KeyContext::StartArray(){
        builder_.StartArray();
        return builder_;
    }

    DictContext KeyContext::StartDict() {
        builder_.StartDict();
        return builder_;
    }

}