#include "global.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_effect.h"
#include "field_effect_helpers.h"
#include "field_player_avatar.h"
#include "field_weather.h"
#include "main.h"
#include "party_menu.h"
#include "sprite.h"
#include "surfable.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/field_effects.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "palette.h"

extern const struct OamData gObjectEventBaseOam_32x32;
extern const struct OamData gObjectEventBaseOam_64x64;
extern const struct SpriteTemplate *const gFieldEffectObjectTemplatePointers[];

extern void SynchroniseSurfAnim(struct ObjectEvent *playerObj, struct Sprite *sprite);
extern void SynchroniseSurfPosition(struct ObjectEvent *playerObj, struct Sprite *sprite);

static void CreateOverlaySprite(bool8 isShiny);
static void UpdateSurfMonOverlay(struct Sprite *sprite);

struct RideablePokemon
{
    u16 species;
    u8 trainerPose;
};

#include "data/object_events/surfable/surfable_pokemon.h"
#include "data/object_events/surfable/surfable_pokemon_graphics.h"
#include "data/object_events/surfable/surfable_pokemon_pic_tables.h"
#include "data/object_events/surfable/surfable_pokemon_templates.h"

static EWRAM_DATA u16 sCurrentSurfMon = {0};

// Hasep: overworld wild Pokémon on water use these swim sprites instead of
// standing on the water (wild_encounter_ow.c). A shiny one gets its own tag:
// the surf palettes share one tag between normal and shiny, which is fine for
// the player's single mount but not for two wild ones of a species.
#define OWE_SWIM_SHINY_PAL_TAG_OFFSET 0x200
STATIC_ASSERT(PAL_TAG_CLODSIRE_SURF < PAL_RAW_REFLECTION_OFFSET, SurfPaletteTagsBelowRawReflectionTags);
STATIC_ASSERT(PAL_TAG_SQUIRTLE_SURF + OWE_SWIM_SHINY_PAL_TAG_OFFSET > PAL_RAW_REFLECTION_OFFSET + 0x10, ShinySwimPaletteTagsAboveRawReflectionTags);

static u16 GetSurfablePokemonIndex(u16 species)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(gSurfablePokemon); i++)
    {
        if (species == gSurfablePokemon[i].species)
            return i;
    }
    return 0xFFFF;
}

bool32 HasSurfablePokemonSprite(u16 species)
{
    return GetSurfablePokemonIndex(species) != 0xFFFF;
}

// Returns MAX_SPRITES if the species has no swim sprite or nothing is free.
u8 CreateSurfablePokemonSpriteForOWE(u16 species, bool32 isShiny, SpriteCallback callback)
{
    u32 index = GetSurfablePokemonIndex(species);
    struct SpritePalette palette;
    u8 paletteNum, spriteId;

    if (index == 0xFFFF)
        return MAX_SPRITES;

    if (!isShiny)
    {
        palette = sSurfablePokemonPalettes[index];
    }
    else
    {
        if (gSaveBlock3Ptr != NULL && gSaveBlock3Ptr->challengeSettings.tx_Features_ShinyColors)
            palette = sSurfablePokemonShinyModernPalettes[index];
        else
            palette = sSurfablePokemonShinyPalettes[index];
        palette.tag += OWE_SWIM_SHINY_PAL_TAG_OFFSET;
    }

    if (IndexOfSpritePaletteTag(palette.tag) == 0xFF)
    {
        paletteNum = LoadSpritePalette(&palette);
        if (paletteNum == 0xFF)
            return MAX_SPRITES;
        UpdateSpritePaletteWithWeather(paletteNum, TRUE);
    }
    paletteNum = IndexOfSpritePaletteTag(palette.tag);

    spriteId = CreateSpriteAtEnd(isShiny ? &gSurfablePokemonOverworldShinySprites[index] : &gSurfablePokemonOverworldSprites[index], 0, 0, 0);
    if (spriteId == MAX_SPRITES)
    {
        FreeOWESwimSpritePaletteIfUnused(palette.tag);
        return MAX_SPRITES;
    }

    gSprites[spriteId].oam.paletteNum = paletteNum;
    gSprites[spriteId].coordOffsetEnabled = TRUE;
    gSprites[spriteId].callback = callback;
    return spriteId;
}

// Frees a swim palette once no sprite uses it; the player's surf mount may
// share the normal one.
void FreeOWESwimSpritePaletteIfUnused(u16 tag)
{
    u32 i;
    u8 paletteNum = IndexOfSpritePaletteTag(tag);

    if (paletteNum == 0xFF)
        return;
    for (i = 0; i < MAX_SPRITES; i++)
    {
        if (gSprites[i].inUse && gSprites[i].oam.paletteNum == paletteNum)
            return;
    }
    FreeSpritePaletteByTag(tag);
}

static u16 GetSurfMonSpecies(void)
{
    u8 i;
	u16 species;
	
	i = VarGet(VAR_SURF_MON_SLOT);
	species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
    return species;
}

static u16 GetSurfablePokemonSprite(void)
{
    u8 i;
    u16 mon = GetSurfMonSpecies();

    for (i = 0; i < ARRAY_COUNT(gSurfablePokemon); i++)
    {
        if (mon == gSurfablePokemon[i].species)
            return i;
    }
    return 0xFFFF;
}

static void LoadSurfOverworldPalette(bool8 isShiny)
{
    u8 palette;

    if (isShiny == TRUE
        && gSaveBlock3Ptr != NULL
        && gSaveBlock3Ptr->challengeSettings.tx_Features_ShinyColors)
        palette = LoadSpritePalette(&sSurfablePokemonShinyModernPalettes[sCurrentSurfMon]);
    else if (isShiny == TRUE)
        palette = LoadSpritePalette(&sSurfablePokemonShinyPalettes[sCurrentSurfMon]);
    else
        palette = LoadSpritePalette(&sSurfablePokemonPalettes[sCurrentSurfMon]);

    UpdateSpritePaletteWithWeather(palette, TRUE);
}

u32 CreateSurfablePokemonSprite(void)
{
    u8 spriteId;
    bool8 isShiny;
    struct Sprite *sprite;

    isShiny = IsMonShiny(&gPlayerParty[VarGet(VAR_SURF_MON_SLOT)]);

    SetSpritePosToOffsetMapCoords((s16 *)&gFieldEffectArguments[0], (s16 *)&gFieldEffectArguments[1], 8, 8);

    sCurrentSurfMon = GetSurfablePokemonSprite();
    if (sCurrentSurfMon != 0xFFFF)
    {
        LoadSurfOverworldPalette(isShiny);

        if (isShiny == TRUE)
            spriteId = CreateSpriteAtEnd(&gSurfablePokemonOverworldShinySprites[sCurrentSurfMon], gFieldEffectArguments[0], gFieldEffectArguments[1], 0x96);
        else
            spriteId = CreateSpriteAtEnd(&gSurfablePokemonOverworldSprites[sCurrentSurfMon], gFieldEffectArguments[0], gFieldEffectArguments[1], 0x96);

        if (gSurfablePokemonOverlaySprites[sCurrentSurfMon].tileTag == 0xFFFF)
        {
            CreateOverlaySprite(isShiny);
        }
    }
    else
    { // Create surf blob
        spriteId = CreateSpriteAtEnd(gFieldEffectObjectTemplatePointers[FLDEFFOBJ_SURF_BLOB], gFieldEffectArguments[0], gFieldEffectArguments[1], 0x96);
    }

    if (spriteId != MAX_SPRITES)
    {
        sprite = &gSprites[spriteId];
        sprite->coordOffsetEnabled = TRUE;
        if (sCurrentSurfMon == 0xFFFF)
            sprite->oam.paletteNum = LoadPlayerObjectEventPalette(gSaveBlock2Ptr->playerGender);
        sprite->data[2] = gFieldEffectArguments[2];
        sprite->data[3] = -1;
        sprite->data[6] = -1;
        sprite->data[7] = -1;
    }
    FieldEffectActiveListRemove(FLDEFF_SURF_BLOB);
    return spriteId;
}

static void CreateOverlaySprite(bool8 isShiny)
{
    u8 overlaySprite;
    u8 subpriority;
    struct Sprite *sprite;

    subpriority = gSprites[gPlayerAvatar.spriteId].subpriority - 1;
    if (isShiny == TRUE)
        overlaySprite = CreateSpriteAtEnd(&gSurfablePokemonOverlayShinySprites[sCurrentSurfMon], gFieldEffectArguments[0], gFieldEffectArguments[1], subpriority);
    else
        overlaySprite = CreateSpriteAtEnd(&gSurfablePokemonOverlaySprites[sCurrentSurfMon], gFieldEffectArguments[0], gFieldEffectArguments[1], subpriority);

    if (overlaySprite != MAX_SPRITES)
    {
        sprite = &gSprites[overlaySprite];
        sprite->coordOffsetEnabled = TRUE;
        sprite->data[2] = gFieldEffectArguments[2];
        sprite->data[3] = -1;
        sprite->data[6] = -1;
        sprite->data[7] = -1;
        sprite->oam.priority = 2;
    }
    SetSurfBlob_BobState(overlaySprite, BOB_PLAYER_AND_MON);
}

static void UpdateSurfMonOverlay(struct Sprite *sprite)
{
    struct ObjectEvent *playerObj;
    struct Sprite *linkedSprite;
    u8 subpriority;
	
    playerObj = &gObjectEvents[gPlayerAvatar.objectEventId];
    linkedSprite = &gSprites[playerObj->spriteId];

    SynchroniseSurfAnim(playerObj, sprite);
    SynchroniseSurfPosition(playerObj, sprite);

    // Ensure Fishing whilst surfing has overlay sprite "bob" up and down appropriately
    if (VarGet(VAR_FREEZE_SURF_BLOB) == 0)
	{
        UpdateBobbingEffect(playerObj, linkedSprite, sprite);
    }

    // Reset the subpriority for the overlay sprite so it shows on top of the player
    // We need this here so the subprio is correct after a screen transition (e.g. after exiting a battle)
    subpriority = gSprites[gPlayerAvatar.spriteId].subpriority - 1;
    sprite->subpriority = subpriority;

    if (linkedSprite->animNum < MOVEMENT_ACTION_DELAY_16)
    {
        sprite->x = linkedSprite->x;
        sprite->y = linkedSprite->y + 8;
        sprite->y2 = linkedSprite->y2;
    }
    if (!(gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_SURFING))
        DestroySprite(sprite);
}

