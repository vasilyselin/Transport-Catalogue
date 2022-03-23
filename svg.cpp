#include "svg.h"

namespace svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << "\\n";
}

std::ostream& operator<<(std::ostream& os, const StrokeLineCap& obj) {
    switch (static_cast<int>(obj))
    {
    case 0 :
        os<<"butt";
        break;
    case 1 :
        os<<"round";
        break;
    case 2 :
        os<<"square";
        break;
    
    default:
        break;
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const StrokeLineJoin& obj) {
    switch (static_cast<int>(obj))
    {
    case 0 :
        os << "arcs";
        break;
    case 1 :
        os << "bevel";
        break;
    case 2 :
        os << "miter";
        break;
    case 3 :
        os << "miter-clip";
        break;
    case 4 :
        os << "round";
        break;
    default:
        break;
    }
    return os;
}    
    
// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\\\""sv << center_.x << "\\\" cy=\\\""sv << center_.y << "\\\""sv;
    out << " r=\\\""sv << radius_ << "\\\""sv;
    RenderAttrs(context.out);
    out << "/>"sv;
}
   
Polyline& Polyline::AddPoint(Point point){
    coordinates_.push_back(point);
    return *this;
}    
    
void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\\\""sv;
    if(!coordinates_.empty()){
        for(size_t i = 0; i< coordinates_.size()-1; ++i){
            out<< coordinates_[i].x << ","sv << coordinates_[i].y << " "sv;
        }
        out<< coordinates_[coordinates_.size()-1].x << ","sv << coordinates_[coordinates_.size()-1].y<< "\\\""sv;
        RenderAttrs(context.out);
        out << "/>"sv;            
    }
    else{
        out << "\\\" />"sv;
    }

}    
   

Text& Text::SetPosition(Point pos){
    pos_ = pos;
    return *this;
}

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
Text& Text::SetOffset(Point offset){
    offset_ = offset;
    return *this;
}

    // Задаёт размеры шрифта (атрибут font-size)
Text& Text::SetFontSize(uint32_t size){
    size_ = size;
    return *this;
}

    // Задаёт название шрифта (атрибут font-family)
Text& Text::SetFontFamily(std::string font_family){
    font_family_ = font_family;
    return *this;
}

    // Задаёт толщину шрифта (атрибут font-weight)
Text& Text::SetFontWeight(std::string font_weight){
    font_weight_ = font_weight;
    return *this;
}

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
Text& Text::SetData(std::string data){
    for(auto i : data){
        if(i == '"'){
            data_+="&quot";
        }
        else if(i == '<'){
            data_+="&lt";
        }
        else if(i == '>'){
            data_+="&gt";
        }        
        else if(i == '&'){
            data_+="&amp";
        }        
        else if(i == '\''){
            data_+="&apos";
        }        
        else{
            data_ += i;
        }
    }    
    return *this;
}
    
void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text";
    RenderAttrs(context.out);
    out << " x=\\\""sv << pos_.x << "\\\" y=\\\""sv << pos_.y << "\\\" "sv;
    out << "dx=\\\""sv << offset_.x << "\\\" dy=\\\""sv << offset_.y << "\\\" "sv;    
    out << "font-size=\\\""sv << size_ << "\\\"";
    if(font_family_ != ""){
        out << " font-family=\\\""sv << font_family_ << "\\\""sv;
    }    
    if(font_weight_ != ""){
        out << " font-weight=\\\""sv << font_weight_ << "\\\""sv;
    }
    out << ">"sv << data_;
    out << "</text>"sv;    
}
    
    void Document::AddPtr(std::unique_ptr<Object>&& obj){
        objects_.emplace_back(std::move(obj));
    }

    // Выводит в ostream svg-представление документа
    void Document::Render(std::ostream& out) const{
        RenderContext render_(out, 2, 2);        
        out << '\n' << '\t' << '{' << '\n' << '\t' << '\t' << "\"map\": \"";
        out << "<?xml version=\\\"1.0\\\" encoding=\\\"UTF-8\\\" ?>\\n";
        out << "<svg xmlns=\\\"http://www.w3.org/2000/svg\\\" version=\\\"1.1\\\">\\n";        
        for(auto& i : objects_){
            i->Render(render_);
        }
        out << "</svg>\""<< ','<< '\n';
        out << '\t' << '\t' << "\"request_id\": " << id_ << '\n';
        out << '\t' << '}'; 
    }
    
    
}  // namespace svg