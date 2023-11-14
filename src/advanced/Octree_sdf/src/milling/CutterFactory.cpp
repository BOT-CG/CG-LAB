#include "CutterFactory.h"

#include "CutterAPT.h"
#include "CutterBallEndmill.h"
#include "CutterFilletEndmill.h"
#include "CutterFilletTaperEndmill.h"
#include "CutterFlatEndmill.h"
#include "CutterStrange.h"
#include "CutterTaperEndmill.h"
#include "CutterTaperFilletEndmill.h"

CutterFactory::Type CutterFactory::getType(float d, float r, float e, float f, float alpha, float beta, float h)
{
    if (isFlatEndmill(d, r, e, f, alpha, beta, h)) {
        return Type::FlatEndmill;
    } else if (isBallEndmill(d, r, e, f, alpha, beta, h)) {
        return Type::BallEndmill;
    } else if (isFilletEndmill(d, r, e, f, alpha, beta, h)) {
        return Type::FilletEndmill;
    } else if (isTaperEndmill(d, r, e, f, alpha, beta, h)) {
        return Type::TaperEndmill;
    } else if (isFilletTaperEndmill(d, r, e, f, alpha, beta, h)) {
        return Type::FilletTaperEndmill;
    } else if (isTaperFilletEndmill(d, r, e, f, alpha, beta, h)) {
        return Type::TaperFilletEndmill;
    } else if (isStrange(d, r, e, f, alpha, beta, h)) {
        return Type::Strange;
    } else {
        return Type::APT;
    }
}

std::unique_ptr<Cutter> CutterFactory::create(float d, float r, float e, float f, float alpha, float beta, float h)
{
    switch (getType(d, r, e, f, alpha, beta, h)) {
    case Type::FlatEndmill:
        return std::make_unique<CutterFlatEndmill>(d, h);
    case Type::FilletEndmill:
        return std::make_unique<CutterFilletEndmill>(d, r, h);
    case Type::FilletTaperEndmill:
        return std::make_unique<CutterFilletTaperEndmill>(d, r, beta, h);
    case Type::BallEndmill:
        return std::make_unique<CutterBallEndmill>(r, h);
    case Type::TaperEndmill:
        return std::make_unique<CutterTaperEndmill>(d, beta, h);
    case Type::TaperFilletEndmill:
        return std::make_unique<CutterTaperFilletEndmill>(d, r, e, f, h);
    case Type::Strange:
        return std::make_unique<CutterStrange>(h);
    case Type::APT:
        return std::make_unique<CutterAPT>(d, r, e, f, alpha, beta, h);
    default:
        return std::make_unique<CutterAPT>(d, r, e, f, alpha, beta, h);
    }
}
