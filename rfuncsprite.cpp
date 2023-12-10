#include "rfuncsprite.h"

RFuncSprite::RFuncSprite()
{

}
void RFuncSprite::Create(const sf::Vector2u& size)
{
    _image.create(size.x, size.y, sf::Color::Cyan); // Выставим цвет по умолчанию
                      _texture.loadFromImage(_image); // Проинициализируем текстуру
                      setTexture(_texture);
    m_rect.resize(300);

    // изменение размера вложенных векторов
    for (auto& inner_vector : m_rect) {
        inner_vector.resize(300);
    }
    _imageX.create(size.x, size.y, sf::Color::Cyan); // Выставим цвет по умолчанию
                      _texture.loadFromImage(_image); // Проинициализируем текстуру
                      setTexture(_texture);
    _imageY.create(size.x, size.y, sf::Color::Cyan); // Выставим цвет по умолчанию
                      _texture.loadFromImage(_image); // Проинициализируем текстуру
                      setTexture(_texture);
   _imageZ.create(size.x, size.y, sf::Color::Cyan); // Выставим цвет по умолчанию
                     _texture.loadFromImage(_image); // Проинициализируем текстуру
                     setTexture(_texture);
   _imageW.create(size.x, size.y, sf::Color::Cyan); // Выставим цвет по умолчанию
                     _texture.loadFromImage(_image); // Проинициализируем текстуру
                     setTexture(_texture);
}

float matrix2Determinant(std::array<std::array<float, 2>, 2> m)
{
    return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}
float matrix3Determinant(std::array<std::array<float, 3>, 3> m)
{
    return m[0][0]*matrix2Determinant({m[1][1], m[1][2], m[2][1], m[2][2]})
          -m[0][1]*matrix2Determinant({m[1][0], m[1][2], m[2][0], m[2][2]})
          +m[0][2]*matrix2Determinant({m[1][0], m[1][1], m[2][0], m[2][1]});
}

void RFuncSprite::computeNormals(int i, int j, const sf::FloatRect& subSpace, sf::Vector2f spaceStep)
{

        const float x1 = subSpace.left + static_cast<float>(j) * spaceStep.x;
        const float y1 = subSpace.top  + static_cast<float>(i) * spaceStep.y;
        const float z1 = m_rect[j][i].z;

        const int j2 = j < (300 - 1) ? (j + 1) : (j - 1);
        const float x2 = subSpace.left + static_cast<float>(j2) * spaceStep.x;
        const float y2 = subSpace.top  + static_cast<float>(i) * spaceStep.y;
        const float z2 = m_rect[j2][i].z;

        const int i3 = i < (300 - 1) ? (i + 1) : (i - 1);
        const float x3 = subSpace.left + static_cast<float>(j) * spaceStep.x;
        const float y3 = subSpace.top  + static_cast<float>(i3) * spaceStep.y;
        const float z3 = m_rect[j][i3].z;

        const float A = matrix3Determinant({y1, z1, 1, y2, z2, 1, y3, z3, 1});
        const float B = matrix3Determinant({x1, z1, 1, x2, z2, 1, x3, z3, 1});
        const float C = matrix3Determinant({x1, y1, 1, x2, y2, 1, x3, y3, 1});
        const float D = matrix3Determinant({x1, y1, z1, x2, y2, z2, x3, y3, z3});

        const float l = std::sqrt(A*A + B*B + C*C + D*D);

        m_rect[j][i].nx =A/l;
        m_rect[j][i].ny =B/l;
        m_rect[j][i].nz =C/l;
        m_rect[j][i].nw =D/l;
}

void RFuncSprite::DrawRFunc(const std::function<float(const sf::Vector2f&)>& rfunc,  const sf::FloatRect& subSpace, int _normalType)
{
    #pragma omp parallel for
    for (unsigned x = 0; x < _image.getSize().x; ++x)
    {
        for (unsigned y = 0; y < _image.getSize().y; ++y)
        {
            sf::Vector2f spaceStep =
            {
                subSpace.width  / static_cast<float>(_image.getSize().x),
                subSpace.height / static_cast<float>(_image.getSize().y),
            };
            sf::Vector2f spacePoint =
            {
                subSpace.left + static_cast<float>(x) * spaceStep.x,
                subSpace.top  + static_cast<float>(y) * spaceStep.y,
            };
            m_rect[x][y].z = rfunc(spacePoint);
            if (m_rect[x][y].z > 0)
            {
                _image.setPixel(x, y, sf::Color::Red);

            }
            else
            {
                _image.setPixel(x, y, sf::Color::Cyan);
            }
        }
    }

    if(_normalType > 0)
    {
        #pragma omp parallel for
        for (unsigned x = 0; x < _image.getSize().x; ++x)
        {
            for (unsigned y = 0; y < _image.getSize().y; ++y)
            {
                computeNormals(y, x, subSpace,{
                                   subSpace.width  / static_cast<float>(_image.getSize().x),
                                   subSpace.height / static_cast<float>(_image.getSize().y),
                               });
                sf::Color pixelColor;
                float rectNormal;
                pixelColor.r = 128*m_rect[x][y].nx + 128;
                pixelColor.g = 128*m_rect[x][y].nx + 128;
                pixelColor.b = 128*m_rect[x][y].nx + 128;
                _imageX.setPixel(x, y, pixelColor);
                pixelColor.r = 128*m_rect[x][y].ny + 128;
                pixelColor.g = 128*m_rect[x][y].ny + 128;
                pixelColor.b = 128*m_rect[x][y].ny + 128;
                _imageY.setPixel(x, y, pixelColor);
                pixelColor.r = 128*m_rect[x][y].nz + 128;
                pixelColor.g = 128*m_rect[x][y].nz + 128;
                pixelColor.b = 128*m_rect[x][y].nz + 128;
                _imageZ.setPixel(x, y, pixelColor);
                pixelColor.r = 128*m_rect[x][y].nw + 128;
                pixelColor.g = 128*m_rect[x][y].nw + 128;
                pixelColor.b = 128*m_rect[x][y].nw + 128;
                _imageW.setPixel(x, y, pixelColor);
            }
        }
    }
    _texture.loadFromImage(_image);

}

void RFuncSprite::render(sf::RenderWindow& window, int imageNum)
{
    sf::Sprite a;
    switch(imageNum)
    {
    case 0:
        window.draw(*this);
        break;
    case 1:

    _texture.loadFromImage(_imageX);
    a.setTexture(_texture);
    window.draw(a);
        break;
    case 2:
    _texture.loadFromImage(_imageY);
    a.setTexture(_texture);
    window.draw(a);
        break;
    case 3:
    _texture.loadFromImage(_imageZ);
    a.setTexture(_texture);
    window.draw(a);
        break;
    case 4:
    _texture.loadFromImage(_imageW);
    a.setTexture(_texture);
    window.draw(a);
        break;
    case 5:
        _texture.loadFromImage(_imageX);
        a.setTexture(_texture);
        window.draw(a);
        a.setPosition({300, 0});
        _texture.loadFromImage(_imageY);
        a.setTexture(_texture);
        window.draw(a);
        a.setPosition({600, 0});
        _texture.loadFromImage(_imageZ);
        a.setTexture(_texture);
        window.draw(a);
        a.setPosition({0, 300});
        _texture.loadFromImage(_imageW);
        a.setTexture(_texture);
        window.draw(a);
        a.setPosition({300, 300});
        _texture.loadFromImage(_image);
        a.setTexture(_texture);
        window.draw(a);
        break;
    }
}
