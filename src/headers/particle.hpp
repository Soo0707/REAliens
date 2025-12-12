#include <cstddef>

#include "raylib.h"

class Particle
{
	public:
		Particle() noexcept;
		~Particle() = default;

		Update() noexcept;
		Draw() const noexcept;

	private:
		float Rotation;
		size_t Creation;
		size_t Expiry;
		Texture2D Image;

};
