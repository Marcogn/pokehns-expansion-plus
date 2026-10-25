#include "global.h"
#include "main.h"
#include "battle.h"
#include "battle_partner.h"
#include "battle_frontier.h"
#include "data.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "frontier_util.h"
#include "load_save.h"
#include "difficulty.h"
#include "string_util.h"
#include "text.h"

#include "constants/abilities.h"
#include "constants/battle_ai.h"

#include "data/partner_parties.h"
#if !TESTING
const struct Trainer gBattlePartners[DIFFICULTY_COUNT][PARTNER_COUNT] =
{
#include "data/battle_partners.h"
};
#endif

#define STEVEN_OTID 61226

// The OT ID a scripted partner's Pokemon get, derived from the partner's name
// (Steven has a fixed one).
static u32 GetPartnerOtId(u16 trainerId, const u8 *partnerName)
{
    u32 k, firstIdPart = 0, secondIdPart = 0, thirdIdPart = 0;

    if (trainerId == TRAINER_PARTNER(PARTNER_STEVEN))
        return STEVEN_OTID;

    for (k = 0; partnerName[k] != EOS && k < 3; k++)
    {
        if (k == 0)
        {
            firstIdPart = partnerName[k];
            secondIdPart = partnerName[k];
            thirdIdPart = partnerName[k];
        }
        else if (k == 1)
        {
            secondIdPart = partnerName[k];
            thirdIdPart = partnerName[k];
        }
        else if (k == 2)
        {
            thirdIdPart = partnerName[k];
        }
    }
    return ((firstIdPart % 72) * 1000) + ((secondIdPart % 23) * 10) + (thirdIdPart % 37) % 65536;
}

// Builds one Pokemon of a battle_partners.party entry. Shared by the multi
// battle partners and by Red's Path, which puts a whole partner party in the
// player's place.
static void CreatePartnerMon(struct Pokemon *mon, const struct Trainer *partner, const struct TrainerMon *partyData, u32 otID)
{
    s32 j;
    u32 personality = Random32();
    u8 trainerName[(PLAYER_NAME_LENGTH * 3) + 1];

    if (partyData->gender == TRAINER_MON_MALE)
        personality = (personality & 0xFFFFFF00) | GeneratePersonalityForGender(MON_MALE, partyData->species);
    else if (partyData->gender == TRAINER_MON_FEMALE)
        personality = (personality & 0xFFFFFF00) | GeneratePersonalityForGender(MON_FEMALE, partyData->species);
    ModifyPersonalityForNature(&personality, partyData->nature);
    CreateMon(mon, partyData->species, partyData->lvl, personality, OTID_STRUCT_PRESET(otID));
    j = partyData->isShiny;
    SetMonData(mon, MON_DATA_IS_SHINY, &j);
    SetMonData(mon, MON_DATA_HELD_ITEM, &partyData->heldItem);
    CustomTrainerPartyAssignMoves(mon, partyData);

    SetMonData(mon, MON_DATA_IVS, &(partyData->iv));
    if (partyData->ev != NULL)
    {
        SetMonData(mon, MON_DATA_HP_EV, &(partyData->ev[0]));
        SetMonData(mon, MON_DATA_ATK_EV, &(partyData->ev[1]));
        SetMonData(mon, MON_DATA_DEF_EV, &(partyData->ev[2]));
        SetMonData(mon, MON_DATA_SPATK_EV, &(partyData->ev[3]));
        SetMonData(mon, MON_DATA_SPDEF_EV, &(partyData->ev[4]));
        SetMonData(mon, MON_DATA_SPEED_EV, &(partyData->ev[5]));
    }
    if (partyData->ability != ABILITY_NONE)
    {
        const struct SpeciesInfo *speciesInfo = &gSpeciesInfo[partyData->species];
        u32 maxAbilities = ARRAY_COUNT(speciesInfo->abilities);
        for (j = 0; j < maxAbilities; j++)
        {
            if (speciesInfo->abilities[j] == partyData->ability)
                break;
        }
        if (j < maxAbilities)
            SetMonData(mon, MON_DATA_ABILITY_NUM, &j);
    }
    SetMonData(mon, MON_DATA_FRIENDSHIP, &(partyData->friendship));
    if (partyData->ball < POKEBALL_COUNT)
    {
        SetMonData(mon, MON_DATA_POKEBALL, &partyData->ball);
    }
    if (partyData->nickname != NULL)
    {
        SetMonData(mon, MON_DATA_NICKNAME, partyData->nickname);
    }
    CalculateMonStats(mon);

    StringCopy(trainerName, partner->trainerName);
    SetMonData(mon, MON_DATA_OT_NAME, trainerName);
    j = partner->gender;
    SetMonData(mon, MON_DATA_OT_GENDER, &j);
}

void FillPartnerParty(u16 trainerId)
{
    s32 i, j;
    u32 ivs, level;
    u16 monId;
    u32 otID;
    u8 trainerName[(PLAYER_NAME_LENGTH * 3) + 1];
    enum DifficultyLevel difficulty = GetBattlePartnerDifficultyLevel(trainerId);
    SetFacilityPtrsGetLevel();

    if (trainerId > TRAINER_PARTNER(PARTNER_NONE))
    {
        const struct Trainer *partner = &gBattlePartners[difficulty][GetPartnerIdFromTrainerId(trainerId)];

        for (i = 0; i < 3; i++)
            ZeroMonData(&gPlayerParty[i + 3]);

        otID = GetPartnerOtId(trainerId, partner->trainerName);
        for (i = 0; i < 3 && i < partner->partySize; i++)
            CreatePartnerMon(&gPlayerParty[i + 3], partner, &partner->party[i], otID);
    }
    else if (trainerId == TRAINER_EREADER)
    {
        // Scrapped, lol.
        trainerName[0] = gGameLanguage;
    }
    else if (trainerId < FRONTIER_TRAINERS_COUNT)
    {
        level = SetFacilityPtrsGetLevel();
        ivs = GetFrontierTrainerFixedIvs(trainerId);
        otID = Random32();
        for (i = 0; i < FRONTIER_MULTI_PARTY_SIZE; i++)
        {
            monId = gSaveBlock2Ptr->frontier.trainerIds[i + 18];
            CreateFacilityMon(&gFacilityTrainerMons[monId], level, ivs, otID, 0, &gPlayerParty[MULTI_PARTY_SIZE + i]);
            for (j = 0; j < PLAYER_NAME_LENGTH + 1; j++)
                trainerName[j] = gFacilityTrainers[trainerId].trainerName[j];
            SetMonData(&gPlayerParty[MULTI_PARTY_SIZE + i], MON_DATA_OT_NAME, &trainerName);
            j = IsFrontierTrainerFemale(trainerId);
            SetMonData(&gPlayerParty[MULTI_PARTY_SIZE + i], MON_DATA_OT_GENDER, &j);
        }
    }
    else if (trainerId < TRAINER_RECORD_MIXING_APPRENTICE)
    {
        trainerId -= TRAINER_RECORD_MIXING_FRIEND;
        for (i = 0; i < FRONTIER_MULTI_PARTY_SIZE; i++)
        {
            struct EmeraldBattleTowerRecord *record = &gSaveBlock2Ptr->frontier.towerRecords[trainerId];
            struct BattleTowerPokemon monData = record->party[gSaveBlock2Ptr->frontier.trainerIds[18 + i]];
            StringCopy(trainerName, record->name);
            if (record->language == LANGUAGE_JAPANESE)
            {
                if (monData.nickname[0] != EXT_CTRL_CODE_BEGIN || monData.nickname[1] != EXT_CTRL_CODE_JPN)
                {
                    monData.nickname[5] = EOS;
                    ConvertInternationalString(monData.nickname, LANGUAGE_JAPANESE);
                }
            }
            else
            {
                if (monData.nickname[0] == EXT_CTRL_CODE_BEGIN && monData.nickname[1] == EXT_CTRL_CODE_JPN)
                    trainerName[5] = EOS;
            }
            CreateBattleTowerMon_HandleLevel(&gPlayerParty[MULTI_PARTY_SIZE + i], &monData, TRUE);
            SetMonData(&gPlayerParty[MULTI_PARTY_SIZE + i], MON_DATA_OT_NAME, trainerName);
            j = IsFrontierTrainerFemale(trainerId + TRAINER_RECORD_MIXING_FRIEND);
            SetMonData(&gPlayerParty[MULTI_PARTY_SIZE + i], MON_DATA_OT_GENDER, &j);
        }
    }
    else
    {
        trainerId -= TRAINER_RECORD_MIXING_APPRENTICE;
        for (i = 0; i < FRONTIER_MULTI_PARTY_SIZE; i++)
        {
            CreateApprenticeMon(&gPlayerParty[MULTI_PARTY_SIZE + i], &gSaveBlock2Ptr->apprentices[trainerId], gSaveBlock2Ptr->frontier.trainerIds[18 + i]);
            j = IsFrontierTrainerFemale(trainerId + TRAINER_RECORD_MIXING_APPRENTICE);
            SetMonData(&gPlayerParty[MULTI_PARTY_SIZE + i], MON_DATA_OT_GENDER, &j);
        }
    }
}

// Red's Path (docs/plans/reds-path.md): the player fights with Red's team of
// that point of his journey, in their own party slots. FillPartnerParty only
// fills slots 3-5 next to the player's; this replaces all six. The script
// saves the player's party first (special SavePlayerParty) and always puts it
// back after the battle (special LoadPlayerParty). The copy is in RAM, in the
// SaveBlock1 party, so the flash keeps the real team until the next save.
void FillRedsPathParty(u32 challenge)
{
    const struct Trainer *red;
    u32 i, otID;

    if (challenge >= REDS_PATH_CHALLENGE_COUNT)
        challenge = 0;
    red = &gBattlePartners[DIFFICULTY_NORMAL][PARTNER_RED_BROCK + challenge];
    otID = GetPartnerOtId(TRAINER_PARTNER(PARTNER_RED_BROCK + challenge), red->trainerName);

    ZeroPlayerPartyMons();
    for (i = 0; i < PARTY_SIZE && i < red->partySize; i++)
        CreatePartnerMon(&gPlayerParty[i], red, &red->party[i], otID);
    CalculatePlayerPartyCount();
}

// special: VAR_0x8004 = challenge, 0 (Brock) to 7 (Giovanni).
void RedsPath_LoadRedsParty(void)
{
    FillRedsPathParty(gSpecialVar_0x8004);
}

// special: puts the player's party back after the battle. The follower was
// last updated on the way back to the field, from Red's team (none, after a
// loss: they have all fainted), and nothing else refreshes it until the next
// map load, so it has to be done here.
void RedsPath_RestorePlayerParty(void)
{
    LoadPlayerParty();
    UpdateFollowingPokemon();
}
