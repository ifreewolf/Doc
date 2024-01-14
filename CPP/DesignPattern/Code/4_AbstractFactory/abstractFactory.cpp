#include <iostream>
using namespace std;

class FrameApi
{
public:
    virtual void draw() = 0;
protected:
    FrameApi() {};
};

class LayoutApi // 分辨率的布局器
{
public:
    virtual void installFrame() = 0;
protected:
    LayoutApi() {}
};

class ComputerFrame : public FrameApi
{
public:
    ComputerFrame(int pins) : m_pins(pins) {

    }

    void draw() {
        std::cout << "现在是pc机的Frame，我适用的分辨率是" << m_pins << std::endl;
    }
private:
    int m_pins;
};

class MobileFrame : public FrameApi
{
public:
    MobileFrame(int pins) : m_pins(pins) {

    }

    void draw() {
        std::cout << "现在是Mobile的Frame，我适用的分辨率是" << m_pins << std::endl;
    }
private:
    int m_pins;
};

class HighLayout : public LayoutApi
{
public:
    HighLayout(int FrameAdapterPins) : m_FrameAdapterPins(FrameAdapterPins) {

    }

    void installFrame() {
        std::cout << "现在是在PC环境下，我们使用的高分辨率布局" << m_FrameAdapterPins << std::endl;
    }
private:
    int m_FrameAdapterPins;
};

class LowLayout : public LayoutApi
{
public:
    LowLayout(int FrameAdapterPins) : m_FrameAdapterPins(FrameAdapterPins) {

    }

    void installFrame() {
        std::cout << "现在是在Mobile环境下，我们使用的低分辨率布局" << m_FrameAdapterPins << std::endl;
    }
private:
    int m_FrameAdapterPins;
};

class AbstractFactory
{
public:
    virtual FrameApi* createFrameApi() = 0;
    virtual LayoutApi* createLayoutApi() = 0;
protected:
    AbstractFactory() {}
};

class Schema1 : public AbstractFactory
{
public:
    FrameApi* createFrameApi() {
        return new ComputerFrame(1024);
    }

    LayoutApi* createLayoutApi() {
        return new HighLayout(1024);
    }
};

class Schema2 : public AbstractFactory
{
public:
    FrameApi* createFrameApi() {
        return new MobileFrame(800);
    }
    LayoutApi* createLayoutApi() {
        return new LowLayout(800);
    }
};

class AdvancedGuiEngineer
{
public:
    void prepareMaterials(AbstractFactory* pSchema) {
        this->pFrame = pSchema->createFrameApi();
        this->pLayout = pSchema->createLayoutApi();
        pFrame->draw();
        pLayout->installFrame();
    }
private:
    FrameApi* pFrame;
    LayoutApi* pLayout;
};

// 你去肯德基，只要你是点的套餐，就一定不会不适配。
int main(int argc, char *argv[])
{
    AdvancedGuiEngineer* pEng = new AdvancedGuiEngineer();
    pEng->prepareMaterials(new Schema1);

    return 0;
}