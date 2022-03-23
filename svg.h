#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>

namespace svg {
    
using namespace std::literals;    

struct Rgb {
    Rgb() = default;
    Rgb(int rd, int grn, int bl) : red(rd), green(grn), blue(bl) {}
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};
    
struct Rgba {
    Rgba() = default;
    Rgba(uint8_t rd, uint8_t grn, uint8_t bl, double opacity_) : red(rd), green(grn), blue(bl), opacity(opacity_) {}    
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double opacity = 1;
};    
    
using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

inline const std::string NoneColor = "none"s;

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};    
    
std::ostream& operator<<(std::ostream& os, const StrokeLineCap& obj);

std::ostream& operator<<(std::ostream& os, const StrokeLineJoin& obj);    
    
struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */

struct ColorVisitor {
    
    std::ostream& out;
    
    void operator()(std::monostate) const {
        out << "none"sv;
    }    
    void operator()(Rgb color) const {
        out << "rgb("sv << static_cast<int>(color.red) << ","sv << static_cast<int>(color.green) << ","sv << static_cast<int>(color.blue) << ")"sv;
    }
    void operator()(Rgba color) const {
        out << "rgba("sv << static_cast<int>(color.red) << ","sv << static_cast<int>(color.green) << ","sv << static_cast<int>(color.blue) << ","sv << color.opacity << ")"sv;
    }
    void operator()(std::string color) const {
        out << color;
    }    
};    
    
template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color&& color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeColor(Color&& color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeWidth(double width) {
        width_ = std::move(width);
        return AsOwner();
    }
    Owner& SetStrokeLineCap(StrokeLineCap line_cap){
        line_cap_ = std::move(line_cap);
        return AsOwner();
    }
    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        line_join_ = std::move(line_join);
        return AsOwner();
    }

protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;

        if (fill_color_) {
            out << " fill=\\\""sv;
            std::visit(ColorVisitor{out}, *fill_color_);
            out << "\\\""sv;
        }
        if (stroke_color_) {
            out << " stroke=\\\""sv;
            std::visit(ColorVisitor{out}, *stroke_color_);
            out << "\\\""sv;
        }
        if (width_) {
            out << " stroke-width=\\\""sv << *width_ << "\\\""sv;
        }      
        if (line_cap_) {
            out << " stroke-linecap=\\\""sv << *line_cap_ << "\\\""sv;
        }   
        if (line_join_) {
            out << " stroke-linejoin=\\\""sv << *line_join_ << "\\\""sv;
        }           
    }

private:
    Owner& AsOwner() {
        // static_cast безопасно преобразует *this к Owner&,
        // если класс Owner — наследник PathProps
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<StrokeLineCap> line_cap_;
    std::optional<StrokeLineJoin> line_join_;
    std::optional<double> width_;
};    
    
    
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
    
    
    
class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object, public PathProps<Circle> {
public:
    Circle() = default;
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_ = {0, 0};
    double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline final : public Object, public PathProps<Polyline> {
public:
    // Добавляет очередную вершину к ломаной линии
    Polyline() = default;
    Polyline& AddPoint(Point point);
private:
    void RenderObject(const RenderContext& context) const override;
    std::vector<Point> coordinates_;
    /*
     * Прочие методы и данные, необходимые для реализации элемента <polyline>
     */
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text final : public Object, public PathProps<Text> {
public:
    Text() = default;
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);
    
private:
    void RenderObject(const RenderContext& context) const override;
    Point pos_ = {0,0};
    Point offset_ = {0,0};
    uint32_t size_ = 1;
    std::string font_weight_ = "";
    std::string font_family_ = "";
    std::string data_ = "";
    // Прочие данные и методы, необходимые для реализации элемента <text>
};
    
class ObjectContainer {
public:    
    template <typename Obj>
    void Add(Obj obj) {
        AddPtr(std::make_unique<Obj>(std::move(obj)));
    }  
    virtual ~ObjectContainer() = default;
    
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
private:
    
};   

class Drawable {
public:
    virtual void Draw(ObjectContainer& container)const = 0;    
    virtual ~Drawable() = default;
};    
    

class Document : public ObjectContainer {
public:    
    // Добавляет в svg-документ объект-наследник svg::Object

    Document(int id) : id_(id){}

    void AddPtr(std::unique_ptr<Object>&& obj) override;

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;
private:
    std::vector<std::unique_ptr<Object>> objects_;
    int id_;
    // Прочие методы и данные, необходимые для реализации класса Document
};

}  // namespace svg