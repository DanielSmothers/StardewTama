# StardewTama Animal Collection

## Purpose

Replace the single-companion Pokémon model with a farm-animal collection. The first collection milestone establishes persistent animal identities and lets the player choose which owned animals are displayed in the farm scenes.

This is a design contract for the initial implementation, not a claim that the feature is already implemented.

## Player-facing behavior

- The collection has two views: **Farm** and **Reserve**.
- Farm contains animals selected to appear in the active farm/building scenes.
- Reserve contains owned animals not currently selected for display.
- The player can inspect an animal and move it between the two views.
- Moving an animal never creates a new animal, deletes it, or resets its state.
- The initial UI should make the current view and the number of animals in each view clear.
- Species filtering, sorting, multiple buildings, and more display locations are later enhancements.

## Important distinction

Farm/Reserve is a display assignment, not a separate life-simulation rule.

An animal's ownership and identity are independent of whether it is currently displayed. Farm and Reserve animals continue to age and retain their individual stats, friendship, variant, and history. Being in Reserve does not automatically pause the animal, reduce its care, or remove production eligibility. Production rules will be designed separately when the economy and housing systems are implemented.

## Data model

Use one central collection of animal instances. Each instance should have:

- A stable, unique instance ID (not the species ID).
- A species ID referring to the species catalog.
- A display assignment (Farm or Reserve).
- Individual state such as nickname, age, needs, friendship, health, variant, and progression.
- Timestamps needed for elapsed-time simulation.

The species catalog describes what an animal is; the instance record describes this particular animal. Rendering must read from the collection and must not own or mutate animal state.

Do not model Farm and Reserve as separate arrays of copied animal records. A move changes only the display assignment on the existing instance.

## Persistence and migration

- Keep the existing hardware, display, touch, RTC, audio, and sprite-loading layers wherever practical.
- Put farm save data in a distinct namespace/schema rather than reusing Pokémon-specific keys as if they represented animals.
- Version the farm save format from its first release.
- Validate counts, IDs, enum values, and record lengths when loading.
- Save assignment changes promptly so a reboot cannot silently move animals.
- Preserve the existing firmware's current save data until the farm save path is ready; do not reinterpret Pokémon save fields as farm animals.

## Initial implementation sequence

1. Add a host-testable animal instance and collection model.
2. Add versioned persistence and tests for create, reload, move, and state preservation.
3. Add the Farm/Reserve collection screen and profile/move interaction.
4. Connect farm scene rendering to the selected Farm animals.
5. Add species catalog entries and original/licensed animal sprites incrementally.

## Acceptance criteria for the first collection milestone

- A new game can own more than one animal.
- Every animal has a stable unique ID independent of species.
- Farm and Reserve views show the same underlying owned collection filtered by assignment.
- Moving an animal in either direction preserves every other field.
- Assignment and animal state survive reboot.
- Invalid or older farm-save data is handled safely.
- Host-side tests cover collection operations and persistence.