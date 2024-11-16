#include "shape.h"
#include <stdexcept>

enum EvolveResult
{
	NoIntersection,
	FoundIntersection,
	StillEvolving
};

class GJK2D
{
	private:
	std::vector<glm::vec2> vertices{};
	glm::vec2 direction{};
	Shape* shapeA;
	Shape* shapeB;

	public:
	GJK2D();

	private:
	glm::vec2 getSupport(Shape* shape, glm::vec2 dir);
	bool addSupport(const glm::vec2& dir);
	glm::vec2 tripleProduct(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);
	EvolveResult evolveSimplex();

	public:
	bool test(Shape* shapeA, Shape* shapeB);
};