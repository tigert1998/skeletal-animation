#include "terrian/simple_terrian.h"

SimpleTerrian::SimpleTerrian() { perlin_noise_.reset(new PerlinNoise(512)); }