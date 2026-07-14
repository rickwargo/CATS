#pragma once

#include <Arduino.h>
#include <NonBlockingRtttl.h>

#include <utility>
#include "PinModule.h"
#include "say.h"

class BuzzerModule : public PinModule, ISubscriber
{
public:
    BuzzerModule(std::string name, short pin) : PinModule(std::move(name), pin) {}

    const char *soundName(char *buffer, size_t bufferSize, short idx)
    {
        while (idx < 0) idx += soundCount;
        const char *sound = sounds[idx % soundCount];
        char *colon = strchr(sound, ':');
        if (colon != nullptr && colon - sound < bufferSize)
        {
            strncpy(buffer, sound, colon - sound);
            buffer[colon - sound] = '\0';
        }
        else
        {
            strcpy(buffer, "generic");
        }
        return buffer;
    }
    const char *soundName(short idx)
    {
        static char buffer[32] = {};
        return soundName(buffer, sizeof(buffer), idx);
    }
    const char *soundName() { return soundName(currentSound); }

    void onEvent(Event& e) override
    {
        switch (e.type) {
        case Event::ToggleMusic:
            handleToggleMusic((short)e.data);
            break;
        case Event::PreviousMusic:
            handlePreviousMusic((short)e.data);
            break;
        case Event::NextMusic:
            handleNextMusic((short)e.data);
            break;
        default:
            break;
        }
    }

protected:
    bool setup() override
    {
        ctx.bus->subscribe(this);
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);

        return true;
    }

    void cycle() override
    {
        playNotes();
    }

private:
    static constexpr const char *arkanoid = "arkanoid:d=4,o=5,b=140:8g6,16p,16g.6,2a#6,32p,8a6,8g6,8f6,8a6,2g6";
    static constexpr const char *mario = "mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6";
    static constexpr const char *tetris = "tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a";
    static constexpr const char *sounds[] = {arkanoid, mario, tetris};
    static constexpr short soundCount = std::size(sounds);

    bool canPlayNotes = false;
    short currentSound = 0;

    void soundToggle()
    {
        canPlayNotes = !canPlayNotes;
    }

    void playNotes()
    {
        if (!canPlayNotes && rtttl::isPlaying())
        {
            rtttl::stop();
            ledcDetach(pin);
            say("Stop playing song #%d", currentSound+1);
        } else if (!rtttl::isPlaying() && canPlayNotes)
        {
            say("Playing song #%d", currentSound+1);
            while (currentSound < 0) currentSound += soundCount; // in case a large negative number is used to index
            rtttl::begin(pin, sounds[currentSound % soundCount]);
        }
        else if (rtttl::isPlaying())
        {
            rtttl::play();
        }
    }

    void handleToggleMusic(short data)
    {
        soundToggle();
    }
    void handlePreviousMusic(short count = 1)
    {
        if (!count) count = 1;
        if (currentSound == 0) currentSound = soundCount;   // loop from beginning to end by count
        currentSound -= count;
        say("Current sound is now #%d", currentSound+1);
    }
    void handleNextMusic(short count = 1)
    {
        if (!count) count = 1;
        currentSound += count;
        if (currentSound >= soundCount) currentSound %= count;   // loop from end to beginning by count
        say("Current sound is now #%d", currentSound+1);
    }
};
