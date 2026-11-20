#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <Preferences.h>

// Farm collection v1. Keep this module independent of rendering and Arduino UI.
static const uint8_t ANIMAL_COLLECTION_SCHEMA = 1;
static const uint8_t ANIMAL_COLLECTION_CAPACITY = 24;
static const uint8_t ANIMAL_NICKNAME_CAPACITY = 16;

enum AnimalDisplayGroup : uint8_t { ANIMAL_FARM = 0, ANIMAL_RESERVE = 1 };

struct AnimalInstance {
  uint32_t instanceId = 0;
  uint16_t speciesId = 0;
  uint8_t displayGroup = ANIMAL_FARM;
  char nickname[ANIMAL_NICKNAME_CAPACITY] = {};
  uint32_t ageMinutes = 0;
  uint8_t fullness = 80;
  uint8_t happiness = 80;
  uint8_t health = 100;
  uint8_t friendship = 0;
  uint8_t variant = 0;
  uint32_t lastUpdatedEpoch = 0;
};

class AnimalCollection {
public:
  bool load();
  bool save();
  void clear();
  uint8_t count() const { return count_; }
  uint8_t countIn(AnimalDisplayGroup group) const;
  const AnimalInstance *at(uint8_t index) const;
  AnimalInstance *find(uint32_t instanceId);
  const AnimalInstance *find(uint32_t instanceId) const;
  uint32_t add(uint16_t speciesId, AnimalDisplayGroup group = ANIMAL_FARM);
  bool move(uint32_t instanceId, AnimalDisplayGroup group);

private:
  struct SaveBlob {
    uint8_t schema;
    uint8_t count;
    uint32_t nextId;
    AnimalInstance animals[ANIMAL_COLLECTION_CAPACITY];
  };
  Preferences prefs_;
  AnimalInstance animals_[ANIMAL_COLLECTION_CAPACITY] = {};
  uint8_t count_ = 0;
  uint32_t nextId_ = 1;
  bool validGroup(uint8_t group) const { return group <= ANIMAL_RESERVE; }
};
