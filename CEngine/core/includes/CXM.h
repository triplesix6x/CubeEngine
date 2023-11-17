//Файл для извлечения значений из матрицы 4x4


#pragma once
#include <DirectXMath.h>

//Извлечение углов поворота
DirectX::XMFLOAT3 ExtractEulerAngles(const DirectX::XMFLOAT4X4& matrix);

//Извлечение перемещения в пространстве
DirectX::XMFLOAT3 ExtractTranslation(const DirectX::XMFLOAT4X4& matrix);

//Извлечение масштаба
DirectX::XMFLOAT3 ExtractScaling(const DirectX::XMFLOAT4X4& matrix);