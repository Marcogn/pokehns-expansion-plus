#include "global.h"
#include "candy_jar.h"
#include "load_save.h"

#define MAX_CANDY_JAR_EXP 99999999

// Ported from Soulgold as-is. The balance is XOR'd with the save's encryption
// key, the way money and berry powder are.
//
// Note for saves written before the field existed: candyJarExp is appended at
// the end of SaveBlock3, so those bytes hold whatever the sector buffer
// happened to contain. Decrypting garbage - or even a clean zero, which
// decrypts to the key itself, a nine-digit balance - would hand out 999 of
// every candy on the first use. That is handled by the save version migration
// in LoadGameSave (SAVE_VERSION 6), not here.

static u32 DecryptCandyJarExp(u32 *exp)
{
    return *exp ^ gSaveBlock2Ptr->encryptionKey;
}

void SetCandyJarExp(u32 *exp, u32 amount)
{
    *exp = amount ^ gSaveBlock2Ptr->encryptionKey;
}

void ApplyNewEncryptionKeyToCandyJarExp(u32 encryptionKey)
{
    ApplyNewEncryptionKeyToWord(&gSaveBlock3Ptr->candyJarExp, encryptionKey);
}

bool8 GiveCandyJarExp(u32 amountToAdd)
{
    u32 *exp = &gSaveBlock3Ptr->candyJarExp;
    u32 amount = DecryptCandyJarExp(exp);

    if (MAX_CANDY_JAR_EXP - amount <= amountToAdd)
    {
        SetCandyJarExp(exp, MAX_CANDY_JAR_EXP);
        return FALSE;
    }

    SetCandyJarExp(exp, amount + amountToAdd);
    return TRUE;
}

bool8 TakeCandyJarExp(u32 amountToTake)
{
    u32 *exp = &gSaveBlock3Ptr->candyJarExp;
    u32 amount = DecryptCandyJarExp(exp);

    if (amount < amountToTake)
        return FALSE;

    SetCandyJarExp(exp, amount - amountToTake);
    return TRUE;
}

u32 GetCandyJarExp(void)
{
    return DecryptCandyJarExp(&gSaveBlock3Ptr->candyJarExp);
}
