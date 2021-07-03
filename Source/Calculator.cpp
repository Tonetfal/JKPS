#include "../Headers/Calculator.hpp"

int Calculator::mKeysPerSecond(0);
int Calculator::mMaxKeyPerSecond(0);
int Calculator::mTotalKeys(0);
int Calculator::mBufferIt(0);
int Calculator::mPressesInCurrentUpdate(0);
const int Calculator::mKPSBufferSize(9);
std::vector<int> Calculator::mKPSBuffer(mKPSBufferSize);

static std::array<int, 60> mBuffer({ 0 });

void Calculator::updateCurrent(sf::Time dt)
{
    if (mBuffer[mBufferIt] > 0)
        mKeysPerSecond -= mBuffer[mBufferIt];

    mBuffer[mBufferIt] = mPressesInCurrentUpdate;
    mKeysPerSecond += mPressesInCurrentUpdate;

    mKPSBuffer[mBufferIt % mKPSBufferSize] = mKeysPerSecond;
    if (mKeysPerSecond > mMaxKeyPerSecond)
        mMaxKeyPerSecond = mKeysPerSecond;

    ++mBufferIt;
    if (mBufferIt == mBuffer.size())
        mBufferIt = 0;

    mPressesInCurrentUpdate = 0;
}

int Calculator::getKeyPerSecond()
{
    return mKeysPerSecond;
}

int Calculator::getMaxKeyPerSecond()
{
    return mMaxKeyPerSecond;
}

int Calculator::getTotalKeys()
{
    return mTotalKeys;
}

int Calculator::getBeatsPerMinute()
{
    float bpm = 0;
    for (auto elem : mKPSBuffer)
        bpm += elem;

    // 60 (seconds) / 4 (1/4 time signature) = 15
    return bpm / mKPSBufferSize * 15;
}

void Calculator::add()
{
    ++mPressesInCurrentUpdate;
    ++mTotalKeys;
}

void Calculator::clear()
{
    mBufferIt = 0;
    mPressesInCurrentUpdate = 0;
    for (auto &elem : mBuffer)
        elem = 0;
    mKeysPerSecond = 0;
    for (auto &elem : mKPSBuffer)
        elem = 0;
}

unsigned Calculator::getCategory() const
{
    return Category::Calculator;
}
