#pragma once
#include <vector>

#include "module/FaceModule.h"
#include "module/Module.h"
#include "ISubscriber.h"
#include "Event.h"

class FaceManager : public Module, public ISubscriber
{
public:
    FaceManager(std::string name, unsigned long cycleCheckTime) : Module(std::move(name), cycleCheckTime) {}

    void onEvent(const Event& e) override
    {
        switch (e.type) {
        case Event::EncoderCCW:
            onPreviousFace();
            break;
        case Event::EncoderCW:
            onNextFace();
            break;
        default:
            break;
        }
    }

protected:
    bool setup() override
    {
        faces.reserve(registry.modules.size());
        for (auto m : registry.modules) {
            if (auto face = dynamic_cast<FaceModule*>(m)) {
                faces.push_back(face);
            }
        }
        faceIterator = faces.begin();

        ctx.bus->subscribe(this);
        switchFace(*faceIterator);   // initialize the active face

        return true;
    }

    virtual void switchFace(FaceModule *face)
    {
        const char *name = ctx.activeFace ? ctx.activeFace->getName() ? ctx.activeFace->getName() : "UNKNOWN" : "Initial Display";
        say("[switchFace] switching from %s to %s", name, face->getName());
        if (ctx.activeFace) ctx.activeFace->onDeactivate();
        ctx.activeFace = face;
        ctx.activeFace->onActivate();
    }

    virtual void onPreviousFace()
    {
        if (faceIterator-1 < faces.begin()) faceIterator = faces.end(); // end points to the slot after the last iterator
        --faceIterator;
        switchFace(*faceIterator);
    }

    virtual void onNextFace()
    {
        if (faceIterator+1 < faces.begin()) faceIterator = faces.end();
        faceIterator = (faceIterator+1) >= faces.end() ? faces.begin() : ++faceIterator;
        switchFace(*faceIterator);
    }

private:
    ModuleRegistry& registry = ModuleRegistry::instance();

    std::vector<FaceModule*> faces;

    std::vector<FaceModule*>::iterator faceIterator;
};
