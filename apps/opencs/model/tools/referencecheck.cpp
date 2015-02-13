#include "referencecheck.hpp"

#include <boost/lexical_cast.hpp>

CSMTools::ReferenceCheckStage::ReferenceCheckStage(
    const CSMWorld::RefCollection& references,
    const CSMWorld::RefIdCollection& referencables,
    const CSMWorld::IdCollection<CSMWorld::Cell>& cells,
    const CSMWorld::IdCollection<ESM::Faction>& factions)
    :
    mReferences(references),
    mReferencables(referencables),
    mCells(cells),
    mFactions(factions)
{
}

void CSMTools::ReferenceCheckStage::perform(int stage, CSMDoc::Messages &messages)
{
    const CSMWorld::Record<CSMWorld::CellRef>& record = mReferences.getRecord(stage);

    if (record.isDeleted())
        return;

    const CSMWorld::CellRef& cellRef = record.get();
    const CSMWorld::UniversalId id(CSMWorld::UniversalId::Type_Reference, cellRef.mId);

    // Check for empty reference id
    if (cellRef.mRefID.empty())
        messages.push_back(std::make_pair(id, " is an empty reference"));

    // Check for non existing referenced object
    if (mReferencables.searchId(cellRef.mRefID) == -1)
        messages.push_back(std::make_pair(id, " is referencing non existing object " + cellRef.mRefID));

    // Check if referenced object is in valid cell
    if (mCells.searchId(cellRef.mCell) == -1)
        messages.push_back(std::make_pair(id, " is referencing object from non existing cell " + cellRef.mCell));

    // If object have owner, check if that owner reference is valid
    if (!cellRef.mOwner.empty() and mReferencables.searchId(cellRef.mOwner) == -1)
        messages.push_back(std::make_pair(id, " has non existing owner " + cellRef.mOwner));

    // If object have creature soul trapped, check if that creature reference is valid
    if (!cellRef.mSoul.empty())
        if (mReferencables.searchId(cellRef.mSoul) == -1)
            messages.push_back(std::make_pair(id, " has non existing trapped soul " + cellRef.mSoul));

    bool hasFaction = !cellRef.mFaction.empty();

    // If object have faction, check if that faction reference is valid
    if (hasFaction)
        if (mFactions.searchId(cellRef.mFaction) == -1)
            messages.push_back(std::make_pair(id, " has non existing faction " + cellRef.mFaction));

    // Check item's faction rank
    if (hasFaction && cellRef.mFactionRank < -1)
        messages.push_back(std::make_pair(id, " has faction set but has invalid faction rank " + cellRef.mFactionRank));
    else if (!hasFaction && cellRef.mFactionRank != -2)
        messages.push_back(std::make_pair(id, " has invalid faction rank " + cellRef.mFactionRank));

    // If door have destination cell, check if that reference is valid
    if (!cellRef.mDestCell.empty())
        if (mCells.searchId(cellRef.mDestCell) == -1)
            messages.push_back(std::make_pair(id, " has non existing destination cell " + cellRef.mDestCell));

    // Check if scale isn't negative
    if (cellRef.mScale < 0)
    {
        std::string str = " has negative scale ";
        str += boost::lexical_cast<std::string>(cellRef.mScale);
        messages.push_back(std::make_pair(id, id.getId() + str));
    }

    // Check if charge isn't negative
    if (cellRef.mChargeFloat < 0)
    {
        std::string str = " has negative charges ";
        str += boost::lexical_cast<std::string>(cellRef.mChargeFloat);
        messages.push_back(std::make_pair(id, id.getId() + str));
    }

    // Check if enchantement points aren't negative or are at full (-1)
    if (cellRef.mEnchantmentCharge < 0 && cellRef.mEnchantmentCharge != -1)
    {
        std::string str = " has negative enchantment points ";
        str += boost::lexical_cast<std::string>(cellRef.mEnchantmentCharge);
        messages.push_back(std::make_pair(id, id.getId() + str));
    }

    // Check if gold value isn't negative
    if (cellRef.mGoldValue < 0)
    {
        std::string str = " has negative gold value ";
        str += cellRef.mGoldValue;
        messages.push_back(std::make_pair(id, id.getId() + str));
    }
}

int CSMTools::ReferenceCheckStage::setup()
{
    return mReferences.getSize();
}