#include "framework.h"
#include "../animal.h"
#include "shim/Preferences.h"

TEST(animal_collection, adds_unique_instances_even_for_same_species) {
  mockNvsReset();
  AnimalCollection c;
  CHECK(c.load());
  uint32_t a = c.add(1);
  uint32_t b = c.add(1, ANIMAL_RESERVE);
  CHECK(a != 0); CHECK(b != 0); CHECK(a != b);
  CHECK_EQ(c.count(), 2);
  CHECK_EQ(c.countIn(ANIMAL_FARM), 1);
  CHECK_EQ(c.countIn(ANIMAL_RESERVE), 1);
}

TEST(animal_collection, moving_changes_assignment_without_changing_animal_state) {
  mockNvsReset();
  AnimalCollection c;
  CHECK(c.load());
  uint32_t id = c.add(4);
  AnimalInstance *a = c.find(id);
  CHECK(a != nullptr);
  a->ageMinutes = 12345; a->fullness = 37; a->happiness = 91;
  a->friendship = 72; a->variant = 1;
  strcpy(a->nickname, "Clucky");
  CHECK(c.save());
  CHECK(c.move(id, ANIMAL_RESERVE));
  const AnimalInstance *moved = c.find(id);
  CHECK_EQ(moved->displayGroup, ANIMAL_RESERVE);
  CHECK_EQ(moved->ageMinutes, 12345u); CHECK_EQ(moved->fullness, 37);
  CHECK_EQ(moved->happiness, 91); CHECK_EQ(moved->friendship, 72);
  CHECK_EQ(moved->variant, 1); CHECK_STREQ(moved->nickname, "Clucky");
}

TEST(animal_collection, persists_identity_and_assignment_across_reload) {
  mockNvsReset();
  AnimalCollection first;
  CHECK(first.load());
  uint32_t id = first.add(7);
  CHECK(first.move(id, ANIMAL_RESERVE));
  AnimalCollection afterReboot;
  CHECK(afterReboot.load());
  CHECK_EQ(afterReboot.count(), 1);
  CHECK_EQ(afterReboot.find(id)->speciesId, 7);
  CHECK_EQ(afterReboot.find(id)->displayGroup, ANIMAL_RESERVE);
}

TEST(animal_collection, rejects_invalid_moves_and_capacity_overflow) {
  mockNvsReset();
  AnimalCollection c;
  CHECK(c.load());
  CHECK_EQ(c.add(0), 0u);
  CHECK(!c.move(999, ANIMAL_RESERVE));
  CHECK(!c.move(999, (AnimalDisplayGroup)9));
  for (uint8_t i = 0; i < ANIMAL_COLLECTION_CAPACITY; ++i) CHECK(c.add(i + 1) != 0);
  CHECK_EQ(c.add(99), 0u);
}
