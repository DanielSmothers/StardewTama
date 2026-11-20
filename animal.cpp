#include "animal.h"

void AnimalCollection::clear() {
  memset(animals_, 0, sizeof(animals_));
  count_ = 0;
  nextId_ = 1;
}

bool AnimalCollection::load() {
  prefs_.begin("stardew", false);
  if (!prefs_.isKey("animals")) { clear(); return true; }
  SaveBlob blob = {};
  const size_t expected = sizeof(blob);
  if (prefs_.getBytes("animals", &blob, expected) != expected ||
      blob.schema != ANIMAL_COLLECTION_SCHEMA ||
      blob.count > ANIMAL_COLLECTION_CAPACITY || blob.nextId == 0) {
    clear();
    return false;
  }
  for (uint8_t i = 0; i < blob.count; ++i) {
    if (blob.animals[i].instanceId == 0 || blob.animals[i].speciesId == 0 ||
        !validGroup(blob.animals[i].displayGroup)) {
      clear();
      return false;
    }
    for (uint8_t j = 0; j < i; ++j) {
      if (blob.animals[i].instanceId == blob.animals[j].instanceId) {
        clear();
        return false;
      }
    }
    blob.animals[i].nickname[ANIMAL_NICKNAME_CAPACITY - 1] = '\0';
  }
  memcpy(animals_, blob.animals, sizeof(animals_));
  count_ = blob.count;
  nextId_ = blob.nextId;
  return true;
}

bool AnimalCollection::save() {
  SaveBlob blob = {};
  blob.schema = ANIMAL_COLLECTION_SCHEMA;
  blob.count = count_;
  blob.nextId = nextId_;
  memcpy(blob.animals, animals_, sizeof(animals_));
  return prefs_.putBytes("animals", &blob, sizeof(blob)) == sizeof(blob);
}

const AnimalInstance *AnimalCollection::at(uint8_t index) const {
  return index < count_ ? &animals_[index] : nullptr;
}

AnimalInstance *AnimalCollection::find(uint32_t id) {
  for (uint8_t i = 0; i < count_; ++i)
    if (animals_[i].instanceId == id) return &animals_[i];
  return nullptr;
}

const AnimalInstance *AnimalCollection::find(uint32_t id) const {
  for (uint8_t i = 0; i < count_; ++i)
    if (animals_[i].instanceId == id) return &animals_[i];
  return nullptr;
}

uint8_t AnimalCollection::countIn(AnimalDisplayGroup group) const {
  if (!validGroup(group)) return 0;
  uint8_t total = 0;
  for (uint8_t i = 0; i < count_; ++i)
    if (animals_[i].displayGroup == group) ++total;
  return total;
}

uint32_t AnimalCollection::add(uint16_t speciesId, AnimalDisplayGroup group) {
  if (!speciesId || !validGroup(group) || count_ >= ANIMAL_COLLECTION_CAPACITY || nextId_ == 0)
    return 0;
  AnimalInstance animal = {};
  animal.instanceId = nextId_++;
  animal.speciesId = speciesId;
  animal.displayGroup = group;
  animals_[count_++] = animal;
  if (!save()) { --count_; --nextId_; return 0; }
  return animal.instanceId;
}

bool AnimalCollection::move(uint32_t id, AnimalDisplayGroup group) {
  if (!validGroup(group)) return false;
  AnimalInstance *animal = find(id);
  if (!animal) return false;
  if (animal->displayGroup == group) return true;
  const uint8_t old = animal->displayGroup;
  animal->displayGroup = group;
  if (!save()) { animal->displayGroup = old; return false; }
  return true;
}
