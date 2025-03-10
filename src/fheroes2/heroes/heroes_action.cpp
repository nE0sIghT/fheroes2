/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cassert>

#include "agg.h"
#include "agg_image.h"
#include "ai.h"
#include "audio.h"
#include "battle.h"
#include "castle.h"
#include "game.h"
#include "game_delays.h"
#include "game_interface.h"
#include "heroes.h"
#include "icn.h"
#include "kingdom.h"
#include "logging.h"
#include "maps_actions.h"
#include "maps_objects.h"
#include "monster.h"
#include "mp2.h"
#include "mus.h"
#include "payment.h"
#include "profit.h"
#include "race.h"
#include "settings.h"
#include "skill.h"
#include "text.h"
#include "tools.h"
#include "translations.h"
#include "world.h"

void ActionToCastle( Heroes & hero, s32 dst_index );
void ActionToHeroes( Heroes & hero, s32 dst_index );
void ActionToMonster( Heroes & hero, s32 dst_index );
void ActionToBoat( Heroes & hero, s32 dst_index );
void ActionToCoast( Heroes & hero, s32 dst_index );
void ActionToWagon( Heroes & hero, s32 dst_index );
void ActionToSkeleton( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToObjectResource( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToPickupResource( const Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToFlotSam( const Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToArtifact( Heroes & hero, s32 dst_index );
void ActionToShipwreckSurvivor( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToShrine( Heroes & hero, s32 dst_index );
void ActionToWitchsHut( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToGoodLuckObject( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToPyramid( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToSign( const Heroes & hero, s32 dst_index );
void ActionToMagicWell( Heroes & hero, int32_t dst_index );
void ActionToTradingPost( const Heroes & hero );
void ActionToPrimarySkillObject( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToPoorMoraleObject( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToGoodMoraleObject( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToExperienceObject( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToTreasureChest( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToAncientLamp( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToTeleports( Heroes & hero, s32 dst_index );
void ActionToWhirlpools( Heroes & hero, s32 dst_index );
void ActionToObservationTower( const Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToCaptureObject( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToDwellingJoinMonster( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToDwellingRecruitMonster( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToDwellingBattleMonster( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToArtesianSpring( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToAbandoneMine( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToXanadu( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToUpgradeArmyObject( Heroes & hero, const MP2::MapObjectType objectType, const std::string & defaultMessage );
void ActionToMagellanMaps( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToEvent( Heroes & hero, s32 dst_index );
void ActionToObelisk( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToTreeKnowledge( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToOracle( const Heroes & hero, const MP2::MapObjectType objectType );
void ActionToDaemonCave( Heroes & hero, const MP2::MapObjectType objectType, int32_t dst_index );
void ActionToAlchemistsTower( Heroes & hero );
void ActionToStables( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToArena( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToSirens( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToJail( const Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToHutMagi( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToEyeMagi( const Heroes & hero, const MP2::MapObjectType objectType );
void ActionToSphinx( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToBarrier( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );
void ActionToTravellersTent( const Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index );

u32 DialogCaptureResourceObject( const std::string & hdr, const std::string & str, u32 res, u32 buttons = Dialog::OK )
{
    const fheroes2::Sprite & sprite = fheroes2::AGG::GetICN( ICN::RESOURCE, Resource::GetIndexSprite2( res ) );
    std::string msg = str;

    // sprite resource with x / day test
    fheroes2::Image sf( sprite.width() + 30, sprite.height() + 14 );
    sf.reset();

    fheroes2::Blit( sprite, sf, 15, 0 );

    std::string perday = _( "%{count} / day" );
    payment_t info = ProfitConditions::FromMine( res );
    const s32 * current = nullptr;

    switch ( res ) {
    case Resource::MERCURY:
        current = &info.mercury;
        break;
    case Resource::WOOD:
        current = &info.wood;
        break;
    case Resource::ORE:
        current = &info.ore;
        break;
    case Resource::SULFUR:
        current = &info.sulfur;
        break;
    case Resource::CRYSTAL:
        current = &info.crystal;
        break;
    case Resource::GEMS:
        current = &info.gems;
        break;
    case Resource::GOLD:
        current = &info.gold;
        break;
    default:
        break;
    }

    if ( current ) {
        StringReplace( perday, "%{count}", *current );

        switch ( *current ) {
        case 1:
            StringReplace( msg, "%{count}", _( "one" ) );
            break;
        case 2:
            StringReplace( msg, "%{count}", _( "two" ) );
            break;
        default:
            StringReplace( msg, "%{count}", *current );
            break;
        }
    }

    Text text( perday, Font::SMALL );
    text.Blit( ( sf.width() - text.w() ) / 2, sf.height() - 12, sf );

    return Dialog::SpriteInfo( hdr, msg, sf, buttons );
}

u32 DialogGoldWithExp( const std::string & hdr, const std::string & msg, u32 count, u32 exp, u32 buttons = Dialog::OK )
{
    const fheroes2::Sprite & gold = fheroes2::AGG::GetICN( ICN::RESOURCE, 6 );
    const fheroes2::Sprite & sprite = fheroes2::AGG::GetICN( ICN::EXPMRL, 4 );
    fheroes2::Image image( sprite.width() + gold.width() + 50, sprite.width() + 10 );
    image.reset();

    fheroes2::Blit( gold, image, 0, image.height() - gold.height() - 12 );
    fheroes2::Blit( sprite, image, gold.width() + 50, 0 );

    Text text( std::to_string( count ), Font::SMALL );
    text.Blit( ( gold.width() - text.w() ) / 2, image.height() - 12, image );
    text.Set( std::to_string( exp ) );
    text.Blit( gold.width() + 50 + ( sprite.width() - text.w() ) / 2, image.height() - 12, image );

    return Dialog::SpriteInfo( hdr, msg, image, buttons );
}

u32 DialogArtifactWithExp( const std::string & hdr, const std::string & msg, const Artifact & art, u32 exp, u32 buttons = Dialog::OK )
{
    const fheroes2::Sprite & sprite = fheroes2::AGG::GetICN( ICN::EXPMRL, 4 );
    const fheroes2::Sprite & border = fheroes2::AGG::GetICN( ICN::RESOURCE, 7 );
    const fheroes2::Sprite & artifact = fheroes2::AGG::GetICN( ICN::ARTIFACT, art.IndexSprite64() );

    fheroes2::Image image( border.width() + sprite.width() + 50, border.height() );
    image.reset();

    fheroes2::Blit( border, image );
    fheroes2::Blit( artifact, image, 5, 5 );
    fheroes2::Blit( sprite, image, border.width() + 50, ( border.height() - sprite.height() ) / 2 );

    Text text( std::to_string( exp ), Font::SMALL );
    text.Blit( border.width() + 50 + ( sprite.width() - text.w() ) / 2, image.height() - 12, image );

    return Dialog::SpriteInfo( hdr, msg, image, buttons );
}

u32 DialogWithExp( const std::string & hdr, const std::string & msg, u32 exp, u32 buttons = Dialog::OK )
{
    const fheroes2::Sprite & sprite = fheroes2::AGG::GetICN( ICN::EXPMRL, 4 );

    fheroes2::Image image( sprite.width(), sprite.height() + 12 );
    image.reset();

    fheroes2::Blit( sprite, image );
    Text text( std::to_string( exp ), Font::SMALL );
    text.Blit( ( sprite.width() - text.w() ) / 2, sprite.height(), image );

    return Dialog::SpriteInfo( hdr, msg, image, buttons );
}

u32 DialogWithArtifactAndGold( const std::string & hdr, const std::string & msg, const Artifact & art, u32 count, u32 buttons = Dialog::OK )
{
    const fheroes2::Sprite & gold = fheroes2::AGG::GetICN( ICN::RESOURCE, 6 );
    const fheroes2::Sprite & border = fheroes2::AGG::GetICN( ICN::RESOURCE, 7 );
    const fheroes2::Sprite & artifact = fheroes2::AGG::GetICN( ICN::ARTIFACT, art.IndexSprite64() );

    fheroes2::Image image( border.width() + gold.width() + 50, border.height() );
    image.reset();

    fheroes2::Blit( border, image );
    fheroes2::Blit( artifact, image, 5, 5 );
    fheroes2::Blit( gold, image, border.width() + 50, ( border.height() - gold.height() ) / 2 );

    Text text( std::to_string( count ), Font::SMALL );
    text.Blit( border.width() + 50 + ( gold.width() - text.w() ) / 2, border.height() - 25, image );

    return Dialog::SpriteInfo( hdr, msg, image, buttons );
}

u32 DialogWithGold( const std::string & hdr, const std::string & msg, u32 count, u32 buttons = Dialog::OK )
{
    const fheroes2::Sprite & gold = fheroes2::AGG::GetICN( ICN::RESOURCE, 6 );

    fheroes2::Image image( gold.width(), gold.height() + 12 );
    image.reset();

    fheroes2::Blit( gold, image );

    Text text( std::to_string( count ), Font::SMALL );
    text.Blit( ( gold.width() - text.w() ) / 2, gold.height(), image );

    return Dialog::SpriteInfo( hdr, msg, image, buttons );
}

u32 DialogMorale( const std::string & hdr, const std::string & msg, bool good, u32 count )
{
    if ( 1 > count )
        count = 1;
    if ( 3 < count )
        count = 3;
    const fheroes2::Sprite & sprite = fheroes2::AGG::GetICN( ICN::EXPMRL, ( good ? 2 : 3 ) );
    u32 offset = sprite.width() * 4 / 3;

    fheroes2::Image image( sprite.width() + offset * ( count - 1 ), sprite.height() );
    image.reset();

    for ( u32 ii = 0; ii < count; ++ii )
        fheroes2::Blit( sprite, image, offset * ii, 0 );

    return Dialog::SpriteInfo( hdr, msg, image );
}

uint32_t DialogLuck( const std::string & hdr, const std::string & msg, bool good, uint32_t count )
{
    if ( 1 > count )
        count = 1;
    else if ( 3 < count )
        count = 3;

    const fheroes2::Sprite & sprite = fheroes2::AGG::GetICN( ICN::EXPMRL, ( good ? 0 : 1 ) );
    const uint32_t offset = sprite.width() * 2;

    fheroes2::Image image( sprite.width() + offset * ( count - 1 ), sprite.height() );
    image.reset();

    for ( uint32_t i = 0; i < count; ++i )
        fheroes2::Blit( sprite, image, offset * i, 0 );

    return Dialog::SpriteInfo( hdr, msg, image );
}

void BattleLose( Heroes & hero, const Battle::Result & res, bool attacker, int color = Color::NONE )
{
    const uint32_t reason = attacker ? res.AttackerResult() : res.DefenderResult();

    AGG::PlaySound( M82::KILLFADE );
    hero.FadeOut();
    hero.SetKillerColor( color );
    hero.SetFreeman( reason );

    Interface::Basic & I = Interface::Basic::Get();
    I.ResetFocus( GameFocus::HEROES );
    I.RedrawFocus();
}

void RecruitMonsterFromTile( Heroes & hero, Maps::Tiles & tile, const std::string & msg, const Troop & troop, bool remove )
{
    if ( !hero.GetArmy().CanJoinTroop( troop ) )
        Dialog::Message( msg, _( "You are unable to recruit at this time, your ranks are full." ), Font::BIG, Dialog::OK );
    else {
        const u32 recruit = Dialog::RecruitMonster( troop.GetMonster(), troop.GetCount(), false, 0 ).GetCount();

        if ( recruit ) {
            if ( remove && recruit == troop.GetCount() ) {
                Game::PlayPickupSound();

                Game::ObjectFadeAnimation::PrepareFadeTask( tile.GetObject(), tile.GetIndex(), -1, true, false );

                tile.MonsterSetCount( 0 );
                tile.RemoveObjectSprite();
                tile.setAsEmpty();

                Game::ObjectFadeAnimation::PerformFadeTask();
            }
            else
                tile.MonsterSetCount( troop.GetCount() - recruit );

            const payment_t paymentCosts = troop.GetMonster().GetCost() * recruit;
            hero.GetKingdom().OddFundsResource( paymentCosts );

            hero.GetArmy().JoinTroop( troop.GetMonster(), recruit );
            hero.MovePointsScaleFixed();

            Interface::Basic::Get().GetStatusWindow().SetRedraw();
        }
    }
}

static void WhirlpoolTroopLooseEffect( Heroes & hero )
{
    Troop * troop = hero.GetArmy().GetWeakestTroop();
    assert( troop );
    if ( !troop )
        return;

    // Whirlpool effect affects heroes only with more than one creature in more than one slot
    if ( hero.GetArmy().GetCount() == 1 && troop->GetCount() == 1 ) {
        return;
    }

    if ( 1 == Rand::Get( 1, 3 ) ) {
        // TODO: Do we really have this dialog in-game in OG?
        Dialog::Message( _( "A whirlpool engulfs your ship." ), _( "Some of your army has fallen overboard." ), Font::BIG, Dialog::OK );

        if ( troop->GetCount() == 1 ) {
            troop->Reset();
        }
        else {
            troop->SetCount( Monster::GetCountFromHitPoints( troop->GetID(), troop->GetHitPoints() - troop->GetHitPoints() * Game::GetWhirlpoolPercent() / 100 ) );
        }

        Interface::Basic::Get().GetStatusWindow().SetRedraw();
    }
}

// action to next cell
void Heroes::Action( int tileIndex, bool isDestination )
{
    // restore the original music after the action is completed
    const Game::MusicRestorer musicRestorer;

    if ( GetKingdom().isControlAI() )
        return AI::HeroesAction( *this, tileIndex, isDestination );

    Maps::Tiles & tile = world.GetTiles( tileIndex );
    const MP2::MapObjectType objectType = tile.GetObject( tileIndex != GetIndex() );

    if ( MUS::FromMapObject( objectType ) != MUS::UNKNOWN )
        AGG::PlayMusic( MUS::FromMapObject( objectType ), false );

    if ( MP2::isActionObject( objectType, isShipMaster() ) ) {
        Interface::StatusWindow::ResetTimer();
        SetModes( ACTION );
    }

    // new format map only
    ListActions * list = world.GetListActions( tileIndex );
    bool cancel_default = false;

    if ( Modes( ACTION ) || list ) {
        // most likely there will be some action, immediately center the map on the hero to avoid subsequent minor screen movements
        Interface::Basic & I = Interface::Basic::Get();

        I.GetGameArea().SetCenter( GetCenter() );

        I.SetRedraw( Interface::REDRAW_GAMEAREA | Interface::REDRAW_RADAR );
        I.Redraw();
    }

    if ( list ) {
        for ( ListActions::const_iterator it = list->begin(); it != list->end(); ++it ) {
            switch ( ( *it )->GetType() ) {
            case ACTION_ACCESS:
                if ( !ActionAccess::Action( static_cast<ActionAccess *>( *it ), tileIndex, *this ) )
                    cancel_default = true;
                break;

            case ACTION_DEFAULT:
                if ( !ActionDefault::Action( static_cast<ActionDefault *>( *it ) ) )
                    cancel_default = true;
                break;

            case ACTION_MESSAGE:
                ActionMessage::Action( static_cast<ActionMessage *>( *it ) );
                break;

            case ACTION_RESOURCES:
                ActionResources::Action( static_cast<ActionResources *>( *it ), *this );
                break;

            case ACTION_ARTIFACT:
                ActionArtifact::Action( static_cast<ActionArtifact *>( *it ), *this );
                break;

            default:
                break;
            }
        }
    }

    /* default actions */
    if ( cancel_default ) {
        if ( MP2::isPickupObject( objectType ) ) {
            Game::ObjectFadeAnimation::PrepareFadeTask( tile.GetObject(), tile.GetIndex(), -1, true, false );

            tile.RemoveObjectSprite();
            tile.QuantityReset();
            tile.setAsEmpty();

            Game::ObjectFadeAnimation::PerformFadeTask();
        }
    }
    else
        switch ( objectType ) {
        case MP2::OBJ_MONSTER:
            ActionToMonster( *this, tileIndex );
            break;

        case MP2::OBJ_CASTLE:
            ActionToCastle( *this, tileIndex );
            break;
        case MP2::OBJ_HEROES:
            ActionToHeroes( *this, tileIndex );
            break;

        case MP2::OBJ_BOAT:
            ActionToBoat( *this, tileIndex );
            break;
        case MP2::OBJ_COAST:
            ActionToCoast( *this, tileIndex );
            break;

            // resource object
        case MP2::OBJ_WINDMILL:
        case MP2::OBJ_WATERWHEEL:
        case MP2::OBJ_MAGICGARDEN:
        case MP2::OBJ_LEANTO:
            ActionToObjectResource( *this, objectType, tileIndex );
            break;

        case MP2::OBJ_WAGON:
            ActionToWagon( *this, tileIndex );
            break;
        case MP2::OBJ_SKELETON:
            ActionToSkeleton( *this, objectType, tileIndex );
            break;

        // pickup object
        case MP2::OBJ_RESOURCE:
        case MP2::OBJ_BOTTLE:
        case MP2::OBJ_CAMPFIRE:
            ActionToPickupResource( *this, objectType, tileIndex );
            break;

        case MP2::OBJ_WATERCHEST:
        case MP2::OBJ_TREASURECHEST:
            ActionToTreasureChest( *this, objectType, tileIndex );
            break;
        case MP2::OBJ_ANCIENTLAMP:
            ActionToAncientLamp( *this, objectType, tileIndex );
            break;
        case MP2::OBJ_FLOTSAM:
            ActionToFlotSam( *this, objectType, tileIndex );
            break;

        case MP2::OBJ_SHIPWRECKSURVIROR:
            ActionToShipwreckSurvivor( *this, objectType, tileIndex );
            break;
        case MP2::OBJ_ARTIFACT:
            ActionToArtifact( *this, tileIndex );
            break;

            // shrine circle
        case MP2::OBJ_SHRINE1:
        case MP2::OBJ_SHRINE2:
        case MP2::OBJ_SHRINE3:
            ActionToShrine( *this, tileIndex );
            break;

        // witchs hut
        case MP2::OBJ_WITCHSHUT:
            ActionToWitchsHut( *this, objectType, tileIndex );
            break;

        // info message
        case MP2::OBJ_SIGN:
            ActionToSign( *this, tileIndex );
            break;

        // luck modification
        case MP2::OBJ_FOUNTAIN:
        case MP2::OBJ_FAERIERING:
        case MP2::OBJ_IDOL:
            ActionToGoodLuckObject( *this, objectType, tileIndex );
            break;

        case MP2::OBJ_PYRAMID:
            ActionToPyramid( *this, objectType, tileIndex );
            break;
        case MP2::OBJ_MAGICWELL:
            ActionToMagicWell( *this, tileIndex );
            break;
        case MP2::OBJ_TRADINGPOST:
            ActionToTradingPost( *this );
            break;

        // primary skill modification
        case MP2::OBJ_FORT:
        case MP2::OBJ_MERCENARYCAMP:
        case MP2::OBJ_DOCTORHUT:
        case MP2::OBJ_STANDINGSTONES:
            ActionToPrimarySkillObject( *this, objectType, tileIndex );
            break;

        // morale modification
        case MP2::OBJ_OASIS:
        case MP2::OBJ_TEMPLE:
        case MP2::OBJ_WATERINGHOLE:
        case MP2::OBJ_BUOY:
            ActionToGoodMoraleObject( *this, objectType, tileIndex );
            break;

        case MP2::OBJ_SHIPWRECK:
        case MP2::OBJ_GRAVEYARD:
        case MP2::OBJ_DERELICTSHIP:
            ActionToPoorMoraleObject( *this, objectType, tileIndex );
            break;

        // experience modification
        case MP2::OBJ_GAZEBO:
            ActionToExperienceObject( *this, objectType, tileIndex );
            break;
        case MP2::OBJ_DAEMONCAVE:
            ActionToDaemonCave( *this, objectType, tileIndex );
            break;

            // teleports
        case MP2::OBJ_STONELITHS:
            ActionToTeleports( *this, tileIndex );
            break;
        case MP2::OBJ_WHIRLPOOL:
            if ( isDestination )
                ActionToWhirlpools( *this, tileIndex );
            break;

        // obsv tower
        case MP2::OBJ_OBSERVATIONTOWER:
            ActionToObservationTower( *this, objectType, tileIndex );
            break;
        case MP2::OBJ_MAGELLANMAPS:
            ActionToMagellanMaps( *this, objectType, tileIndex );
            break;

        // capture color object
        case MP2::OBJ_ALCHEMYLAB:
        case MP2::OBJ_MINES:
        case MP2::OBJ_SAWMILL:
        case MP2::OBJ_LIGHTHOUSE:
            ActionToCaptureObject( *this, objectType, tileIndex );
            break;

        case MP2::OBJ_ABANDONEDMINE:
            ActionToAbandoneMine( *this, objectType, tileIndex );
            break;

            // accept army
        case MP2::OBJ_WATCHTOWER:
        case MP2::OBJ_EXCAVATION:
        case MP2::OBJ_CAVE:
        case MP2::OBJ_TREEHOUSE:
        case MP2::OBJ_ARCHERHOUSE:
        case MP2::OBJ_GOBLINHUT:
        case MP2::OBJ_DWARFCOTT:
        case MP2::OBJ_HALFLINGHOLE:
        case MP2::OBJ_PEASANTHUT:
        case MP2::OBJ_THATCHEDHUT:
            ActionToDwellingJoinMonster( *this, objectType, tileIndex );
            break;

            // recruit army
        case MP2::OBJ_RUINS:
        case MP2::OBJ_TREECITY:
        case MP2::OBJ_WAGONCAMP:
        case MP2::OBJ_DESERTTENT:
            ActionToDwellingRecruitMonster( *this, objectType, tileIndex );
            break;

        // battle and recruit army
        case MP2::OBJ_DRAGONCITY:
        case MP2::OBJ_CITYDEAD:
        case MP2::OBJ_TROLLBRIDGE:
            ActionToDwellingBattleMonster( *this, objectType, tileIndex );
            break;

        case MP2::OBJ_ARTESIANSPRING:
            ActionToArtesianSpring( *this, objectType, tileIndex );
            break;

        case MP2::OBJ_XANADU:
            ActionToXanadu( *this, objectType, tileIndex );
            break;

        case MP2::OBJ_HILLFORT:
        case MP2::OBJ_FREEMANFOUNDRY:
            ActionToUpgradeArmyObject( *this, objectType, "" );
            break;

        case MP2::OBJ_EVENT:
            ActionToEvent( *this, tileIndex );
            break;

        case MP2::OBJ_OBELISK:
            ActionToObelisk( *this, objectType, tileIndex );
            break;

        case MP2::OBJ_TREEKNOWLEDGE:
            ActionToTreeKnowledge( *this, objectType, tileIndex );
            break;

        case MP2::OBJ_ORACLE:
            ActionToOracle( *this, objectType );
            break;
        case MP2::OBJ_SPHINX:
            ActionToSphinx( *this, objectType, tileIndex );
            break;

            // loyalty version
        case MP2::OBJ_WATERALTAR:
        case MP2::OBJ_AIRALTAR:
        case MP2::OBJ_FIREALTAR:
        case MP2::OBJ_EARTHALTAR:
        case MP2::OBJ_BARROWMOUNDS:
            ActionToDwellingRecruitMonster( *this, objectType, tileIndex );
            break;
        case MP2::OBJ_ALCHEMYTOWER:
            ActionToAlchemistsTower( *this );
            break;
        case MP2::OBJ_STABLES:
            ActionToStables( *this, objectType, tileIndex );
            break;
        case MP2::OBJ_ARENA:
            ActionToArena( *this, objectType, tileIndex );
            break;
        case MP2::OBJ_MERMAID:
            ActionToGoodLuckObject( *this, objectType, tileIndex );
            break;
        case MP2::OBJ_SIRENS:
            ActionToSirens( *this, objectType, tileIndex );
            break;
        case MP2::OBJ_JAIL:
            ActionToJail( *this, objectType, tileIndex );
            break;
        case MP2::OBJ_HUTMAGI:
            ActionToHutMagi( *this, objectType, tileIndex );
            break;
        case MP2::OBJ_EYEMAGI:
            ActionToEyeMagi( *this, objectType );
            break;

        case MP2::OBJ_BARRIER:
            ActionToBarrier( *this, objectType, tileIndex );
            break;
        case MP2::OBJ_TRAVELLERTENT:
            ActionToTravellersTent( *this, objectType, tileIndex );
            break;

            // object
        default:
            break;
        }
}

void ActionToMonster( Heroes & hero, s32 dst_index )
{
    Maps::Tiles & tile = world.GetTiles( dst_index );
    Troop troop = tile.QuantityTroop();

    Interface::Basic & I = Interface::Basic::Get();

    bool destroy = false;

    const NeutralMonsterJoiningCondition join = Army::GetJoinSolution( hero, tile, troop );

    if ( join.reason == NeutralMonsterJoiningCondition::Reason::Alliance ) {
        if ( hero.GetArmy().CanJoinTroop( troop ) ) {
            assert( join.joiningMessage != nullptr );
            Dialog::Message( "", join.joiningMessage, Font::BIG, Dialog::OK );
            hero.GetArmy().JoinTroop( troop );
        }
        else {
            assert( join.fleeingMessage != nullptr );
            Dialog::Message( "", join.fleeingMessage, Font::BIG, Dialog::OK );
        }

        destroy = true;
    }
    else if ( join.reason == NeutralMonsterJoiningCondition::Reason::Bane ) {
        assert( join.fleeingMessage != nullptr );
        Dialog::Message( "", join.fleeingMessage, Font::BIG, Dialog::OK );
        destroy = true;
    }
    else if ( join.reason == NeutralMonsterJoiningCondition::Reason::Free ) {
        DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() << " join monster " << troop.GetName() );

        if ( Dialog::YES == Dialog::ArmyJoinFree( troop, hero ) ) {
            hero.GetArmy().JoinTroop( troop );

            I.GetStatusWindow().SetRedraw();
            destroy = true;
        }
        else {
            Dialog::Message( "", _( "Insulted by your refusal of their offer, the monsters attack!" ), Font::BIG, Dialog::OK );
        }
    }
    else if ( join.reason == NeutralMonsterJoiningCondition::Reason::ForMoney ) {
        const int32_t joiningCost = troop.GetCost().gold;

        if ( Dialog::YES == Dialog::ArmyJoinWithCost( troop, join.monsterCount, joiningCost, hero ) ) {
            DEBUG_LOG( DBG_GAME, DBG_INFO, join.monsterCount << " " << troop.GetName() << " join " << hero.GetName() << " for " << joiningCost << " gold." );

            hero.GetArmy().JoinTroop( troop.GetMonster(), join.monsterCount );
            hero.GetKingdom().OddFundsResource( Funds( Resource::GOLD, joiningCost ) );

            I.GetStatusWindow().SetRedraw();
            destroy = true;
        }
        else {
            Dialog::Message( "", _( "Insulted by your refusal of their offer, the monsters attack!" ), Font::BIG, Dialog::OK );
        }
    }
    else if ( join.reason == NeutralMonsterJoiningCondition::Reason::RunAway ) {
        std::string message = _( "The %{monster}, awed by the power of your forces, begin to scatter.\nDo you wish to pursue and engage them?" );
        StringReplace( message, "%{monster}", StringLower( troop.GetMultiName() ) );

        if ( Dialog::Message( "", message, Font::BIG, Dialog::YES | Dialog::NO ) == Dialog::NO ) {
            destroy = true;
        }
    }

    // fight
    if ( !destroy ) {
        DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() << " attack monster " << troop.GetName() );

        // set the hero's attacked monster tile index and immediately redraw game area to show an attacking sprite for this monster
        hero.SetAttackedMonsterTileIndex( dst_index );

        I.SetRedraw( Interface::REDRAW_GAMEAREA );
        I.Redraw();

        Army army( tile );

        Battle::Result res = Battle::Loader( hero.GetArmy(), army, dst_index );

        if ( res.AttackerWins() ) {
            hero.IncreaseExperience( res.GetExperienceAttacker() );

            destroy = true;
        }
        else {
            BattleLose( hero, res, true );

            tile.MonsterSetCount( army.GetCountMonsters( troop.GetMonster() ) );

            // reset join condition
            if ( tile.MonsterJoinConditionFree() ) {
                tile.MonsterSetJoinCondition( Monster::JOIN_CONDITION_MONEY );
            }
        }
    }

    if ( destroy ) {
        AGG::PlaySound( M82::KILLFADE );

        Game::ObjectFadeAnimation::PrepareFadeTask( tile.GetObject(), tile.GetIndex(), -1, true, false );

        tile.RemoveObjectSprite();
        tile.MonsterSetCount( 0 );
        tile.setAsEmpty();

        Game::ObjectFadeAnimation::PerformFadeTask();
    }

    // clear the hero's attacked monster tile index
    hero.SetAttackedMonsterTileIndex( -1 );
}

void ActionToHeroes( Heroes & hero, s32 dst_index )
{
    Heroes * other_hero = world.GetTiles( dst_index ).GetHeroes();

    if ( !other_hero )
        return;

    if ( hero.GetColor() == other_hero->GetColor() ) {
        DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() << " meeting " << other_hero->GetName() );
        hero.MeetingDialog( *other_hero );
    }
    else if ( hero.isFriends( other_hero->GetColor() ) ) {
        DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() << " disable meeting" );
    }
    else {
        const Castle * other_hero_castle = other_hero->inCastle();
        if ( other_hero_castle && other_hero == other_hero_castle->GetHeroes().GuardFirst() ) {
            ActionToCastle( hero, dst_index );
            return;
        }

        DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() << " attack enemy hero " << other_hero->GetName() );

        // new battle
        Battle::Result res = Battle::Loader( hero.GetArmy(), other_hero->GetArmy(), dst_index );

        // loss defender
        if ( !res.DefenderWins() )
            BattleLose( *other_hero, res, false, hero.GetColor() );

        // loss attacker
        if ( !res.AttackerWins() )
            BattleLose( hero, res, true, other_hero->GetColor() );

        // wins attacker
        if ( res.AttackerWins() ) {
            hero.IncreaseExperience( res.GetExperienceAttacker() );
        }
        else
            // wins defender
            if ( res.DefenderWins() ) {
            other_hero->IncreaseExperience( res.GetExperienceDefender() );
        }
    }
}

void ActionToCastle( Heroes & hero, s32 dst_index )
{
    Castle * castle = world.getCastleEntrance( Maps::GetPoint( dst_index ) );

    if ( !castle ) {
        DEBUG_LOG( DBG_GAME, DBG_INFO, "castle not found " << dst_index );
    }
    else if ( hero.GetColor() == castle->GetColor() ) {
        DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() << " goto castle " << castle->GetName() );
        castle->MageGuildEducateHero( hero );
        Game::OpenCastleDialog( *castle );
    }
    else if ( hero.isFriends( castle->GetColor() ) ) {
        DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() << " disable visiting" );
    }
    else {
        CastleHeroes heroes = castle->GetHeroes();

        // first attack to guest hero
        if ( heroes.FullHouse() ) {
            ActionToHeroes( hero, dst_index );
            return;
        }

        Army & army = castle->GetActualArmy();

        if ( army.isValid() && army.GetColor() != hero.GetColor() ) {
            DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() << " attack enemy castle " << castle->GetName() );

            Heroes * defender = heroes.GuardFirst();
            castle->ActionPreBattle();

            // new battle
            Battle::Result res = Battle::Loader( hero.GetArmy(), army, dst_index );

            castle->ActionAfterBattle( res.AttackerWins() );

            // loss defender
            if ( !res.DefenderWins() && defender )
                BattleLose( *defender, res, false, hero.GetColor() );

            // loss attacker
            if ( !res.AttackerWins() )
                BattleLose( hero, res, true, castle->GetColor() );

            // wins attacker
            if ( res.AttackerWins() ) {
                castle->GetKingdom().RemoveCastle( castle );
                hero.GetKingdom().AddCastle( castle );
                world.CaptureObject( dst_index, hero.GetColor() );
                castle->Scoute();
                Interface::Basic::Get().SetRedraw( Interface::REDRAW_CASTLES );

                hero.IncreaseExperience( res.GetExperienceAttacker() );
            }
            else
                // wins defender
                if ( res.DefenderWins() && defender ) {
                defender->IncreaseExperience( res.GetExperienceDefender() );
            }
        }
        else {
            DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() << " capture enemy castle " << castle->GetName() );

            castle->GetKingdom().RemoveCastle( castle );
            hero.GetKingdom().AddCastle( castle );
            world.CaptureObject( dst_index, hero.GetColor() );
            castle->Scoute();
            Interface::Basic::Get().SetRedraw( Interface::REDRAW_CASTLES );

            castle->MageGuildEducateHero( hero );
            Game::OpenCastleDialog( *castle );
        }
    }
}

void ActionToBoat( Heroes & hero, s32 dst_index )
{
    // If the hero is already on a ship do nothing
    if ( hero.isShipMaster() )
        return;

    hero.setLastGroundRegion( world.GetTiles( hero.GetIndex() ).GetRegion() );

    const fheroes2::Point & destPos = Maps::GetPoint( dst_index );
    const fheroes2::Point offset( destPos - hero.GetCenter() );

    // Get the direction of the boat so that the direction of the hero can be set to it after boarding
    const Maps::Tiles & from = world.GetTiles( dst_index );
    const int boatDirection = from.getBoatDirection();

    AGG::PlaySound( M82::KILLFADE );
    hero.GetPath().Hide();
    hero.FadeOut( offset );
    hero.ResetMovePoints();
    hero.Move2Dest( dst_index );
    // Set the direction of the hero to the one of the boat as the boat does not move when boarding it
    hero.setDirection( boatDirection );
    hero.SetMapsObject( MP2::OBJ_ZERO );
    world.GetTiles( dst_index ).resetObjectSprite();
    hero.SetShipMaster( true );
    hero.GetPath().Reset();

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToCoast( Heroes & hero, s32 dst_index )
{
    if ( !hero.isShipMaster() )
        return;

    const int fromIndex = hero.GetIndex();
    Maps::Tiles & from = world.GetTiles( fromIndex );

    const fheroes2::Point & destPos = Maps::GetPoint( dst_index );
    const fheroes2::Point offset( destPos - hero.GetCenter() );

    hero.ResetMovePoints();
    hero.Move2Dest( dst_index );
    from.setBoat( Maps::GetDirection( fromIndex, dst_index ) );
    hero.SetShipMaster( false );
    AGG::PlaySound( M82::KILLFADE );
    hero.GetPath().Hide();
    hero.FadeIn( fheroes2::Point( offset.x * Game::HumanHeroAnimSkip(), offset.y * Game::HumanHeroAnimSkip() ) );
    hero.GetPath().Reset();
    hero.ActionNewPosition( true );

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToPickupResource( const Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    Maps::Tiles & tile = world.GetTiles( dst_index );

    if ( objectType == MP2::OBJ_BOTTLE ) {
        const MapSign * sign = dynamic_cast<MapSign *>( world.GetMapObject( dst_index ) );
        Dialog::Message( MP2::StringObject( objectType ), ( sign ? sign->message : "No message provided" ), Font::BIG, Dialog::OK );
    }
    else {
        Funds funds = tile.QuantityFunds();

        if ( objectType == MP2::OBJ_CAMPFIRE ) {
            Dialog::ResourceInfo( MP2::StringObject( objectType ), _( "Ransacking an enemy camp, you discover a hidden cache of treasures." ), funds );
        }
        else {
            ResourceCount rc = tile.QuantityResourceCount();

            Interface::Basic & I = Interface::Basic::Get();
            I.GetStatusWindow().SetResource( rc.first, rc.second );
            I.SetRedraw( Interface::REDRAW_STATUS );
        }

        hero.GetKingdom().AddFundsResource( funds );
    }

    Game::PlayPickupSound();

    Game::ObjectFadeAnimation::PrepareFadeTask( tile.GetObject(), tile.GetIndex(), -1, true, false );

    tile.RemoveObjectSprite();
    tile.QuantityReset();

    Game::ObjectFadeAnimation::PerformFadeTask();

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToObjectResource( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    Maps::Tiles & tile = world.GetTiles( dst_index );
    ResourceCount rc = tile.QuantityResourceCount();
    bool cancapture = Settings::Get().ExtWorldExtObjectsCaptured();
    bool showinvalid = cancapture && hero.GetColor() == tile.QuantityColor() ? false : true;

    std::string msg;
    const std::string & caption = MP2::StringObject( objectType );

    // dialog
    switch ( objectType ) {
    case MP2::OBJ_WINDMILL:
        msg = rc.isValid()
                  ? _( "The keeper of the mill announces:\n\"Milord, I have been working very hard to provide you with these resources, come back next week for more.\"" )
                  : _( "The keeper of the mill announces:\n\"Milord, I am sorry, there are no resources currently available. Please try again next week.\"" );
        break;

    case MP2::OBJ_WATERWHEEL:
        msg = rc.isValid()
                  ? _( "The keeper of the mill announces:\n\"Milord, I have been working very hard to provide you with this gold, come back next week for more.\"" )
                  : _( "The keeper of the mill announces:\n\"Milord, I am sorry, there is no gold currently available. Please try again next week.\"" );
        break;

    case MP2::OBJ_LEANTO:
        cancapture = false;
        msg = rc.isValid() ? _( "You've found an abandoned lean-to.\nPoking about, you discover some resources hidden nearby." )
                           : _( "The lean-to is long abandoned. There is nothing of value here." );
        break;

    case MP2::OBJ_MAGICGARDEN:
        msg = rc.isValid()
                  ? _(
                      "You catch a leprechaun foolishly sleeping amidst a cluster of magic mushrooms.\nIn exchange for his freedom, he guides you to a small pot filled with precious things." )
                  : _(
                      "You've found a magic garden, the kind of place that leprechauns and faeries like to cavort in, but there is no one here today.\nPerhaps you should try again next week." );
        break;

    default:
        cancapture = false;
        break;
    }

    if ( rc.isValid() ) {
        const Funds funds( rc );
        AGG::PlaySound( M82::TREASURE );
        Dialog::ResourceInfo( caption, msg, funds );
        hero.GetKingdom().AddFundsResource( funds );

        if ( cancapture )
            ActionToCaptureObject( hero, objectType, dst_index );
    }
    else {
        if ( cancapture )
            ActionToCaptureObject( hero, objectType, dst_index );

        if ( showinvalid )
            Dialog::Message( caption, msg, Font::BIG, Dialog::OK );
    }

    tile.QuantityReset();
    hero.setVisitedForAllies( dst_index );

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToSkeleton( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    Maps::Tiles & tile = world.GetTiles( dst_index );
    std::string message( _( "You come upon the remains of an unfortunate adventurer." ) );
    const std::string title( MP2::StringObject( objectType ) );

    // artifact
    if ( tile.QuantityIsValid() ) {
        Game::PlayPickupSound();

        if ( hero.IsFullBagArtifacts() ) {
            u32 gold = GoldInsteadArtifact( objectType );
            const Funds funds( Resource::GOLD, gold );
            Dialog::ResourceInfo( title, _( "Treasure" ), funds, Dialog::OK );
            hero.GetKingdom().AddFundsResource( funds );
        }
        else {
            const Artifact & art = tile.QuantityArtifact();
            message += '\n';
            message.append( _( "Searching through the tattered clothing, you find the %{artifact}." ) );
            StringReplace( message, "%{artifact}", art.GetName() );
            AGG::PlaySound( M82::TREASURE );
            Dialog::ArtifactInfo( title, message, art );
            hero.PickupArtifact( art );
        }

        tile.QuantityReset();
    }
    else {
        message += '\n';
        message.append( _( "Searching through the tattered clothing, you find nothing." ) );
        Dialog::Message( title, message, Font::BIG, Dialog::OK );
    }

    hero.SetVisitedWideTile( dst_index, objectType, Visit::GLOBAL );

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToWagon( Heroes & hero, s32 dst_index )
{
    Maps::Tiles & tile = world.GetTiles( dst_index );
    std::string message( _( "You come across an old wagon left by a trader who didn't quite make it to safe terrain." ) );
    const std::string title( MP2::StringObject( MP2::OBJ_WAGON ) );

    if ( tile.QuantityIsValid() ) {
        const Artifact & art = tile.QuantityArtifact();

        if ( art.isValid() ) {
            if ( hero.IsFullBagArtifacts() ) {
                message += '\n';
                message.append( _( "Unfortunately, others have found it first, and the wagon is empty." ) );
                Dialog::Message( title, message, Font::BIG, Dialog::OK );
            }
            else {
                message += '\n';
                message.append( _( "Searching inside, you find the %{artifact}." ) );
                StringReplace( message, "%{artifact}", art.GetName() );
                AGG::PlaySound( M82::TREASURE );
                Dialog::ArtifactInfo( title, message, art );
                hero.PickupArtifact( art );
            }
        }
        else {
            const Funds & funds = tile.QuantityFunds();
            AGG::PlaySound( M82::EXPERNCE );
            message += '\n';
            message.append( _( "Inside, you find some of the wagon's cargo still intact." ) );
            Dialog::ResourceInfo( title, message, funds );
            hero.GetKingdom().AddFundsResource( funds );
        }

        tile.QuantityReset();
    }
    else {
        message += '\n';
        message.append( _( "Unfortunately, others have found it first, and the wagon is empty." ) );
        Dialog::Message( title, message, Font::BIG, Dialog::OK );
    }

    hero.SetVisited( dst_index, Visit::GLOBAL );

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToFlotSam( const Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    Maps::Tiles & tile = world.GetTiles( dst_index );
    std::string msg;
    const std::string title( MP2::StringObject( objectType ) );

    const Funds & funds = tile.QuantityFunds();

    if ( 0 < funds.GetValidItemsCount() ) {
        msg = funds.wood && funds.gold ? _( "You search through the flotsam, and find some wood and some gold." )
                                       : _( "You search through the flotsam, and find some wood." );
        Dialog::ResourceInfo( title, msg, funds );
        hero.GetKingdom().AddFundsResource( funds );
    }
    else {
        msg = _( "You search through the flotsam, but find nothing." );
        Dialog::Message( title, msg, Font::BIG, Dialog::OK );
    }

    Game::PlayPickupSound();

    Game::ObjectFadeAnimation::PrepareFadeTask( tile.GetObject(), tile.GetIndex(), -1, true, false );

    tile.RemoveObjectSprite();
    tile.QuantityReset();

    Game::ObjectFadeAnimation::PerformFadeTask();

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToShrine( Heroes & hero, s32 dst_index )
{
    const Spell & spell = world.GetTiles( dst_index ).QuantitySpell();
    const u32 spell_level = spell.Level();

    std::string head;
    std::string body;

    switch ( spell_level ) {
    case 1:
        head = _( "Shrine of the 1st Circle" );
        body = _(
            "You come across a small shrine attended by a group of novice acolytes.\nIn exchange for your protection, they agree to teach you a simple spell - '%{spell}'." );
        break;
    case 2:
        head = _( "Shrine of the 2nd Circle" );
        body = _(
            "You come across an ornate shrine attended by a group of rotund friars.\nIn exchange for your protection, they agree to teach you a spell - '%{spell}'." );
        break;
    case 3:
        head = _( "Shrine of the 3rd Circle" );
        body = _(
            "You come across a lavish shrine attended by a group of high priests.\nIn exchange for your protection, they agree to teach you a sophisticated spell - '%{spell}'." );
        break;
    default:
        // Did you add a new level shrine? Add the logic above.
        assert( 0 );
        return;
    }

    StringReplace( body, "%{spell}", spell.GetName() );

    // check spell book
    if ( !hero.HaveSpellBook() ) {
        if ( !Settings::Get().ExtHeroBuySpellBookFromShrine() || !hero.BuySpellBook( nullptr, spell_level ) ) {
            body += _( "\nUnfortunately, you have no Magic Book to record the spell with." );
            Dialog::Message( head, body, Font::BIG, Dialog::OK );
        }
    }

    if ( hero.HaveSpellBook() ) {
        // check valid level spell and wisdom skill
        if ( 3 == spell_level && Skill::Level::NONE == hero.GetLevelSkill( Skill::Secondary::WISDOM ) ) {
            body += _( "\nUnfortunately, you do not have the wisdom to understand the spell, and you are unable to learn it." );
            Dialog::Message( head, body, Font::BIG, Dialog::OK );
        }
        else
            // already know (skip bag artifacts)
            if ( hero.HaveSpell( spell.GetID(), true ) ) {
            body += _( "\nUnfortunately, you already have knowledge of this spell, so there is nothing more for them to teach you." );
            Dialog::Message( head, body, Font::BIG, Dialog::OK );
        }
        else {
            AGG::PlaySound( M82::TREASURE );
            hero.AppendSpellToBook( spell.GetID() );
            Dialog::SpellInfo( head, body, spell.GetID() );
        }
    }

    hero.SetVisited( dst_index, Visit::GLOBAL );
    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToWitchsHut( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    const Skill::Secondary & skill = world.GetTiles( dst_index ).QuantitySkill();

    AGG::PlayMusic( MUS::SKILL, false );

    // If this assertion blows up the object is not set properly.
    assert( skill.isValid() );

    if ( skill.isValid() ) {
        std::string msg = _( "You approach the hut and observe a witch inside studying an ancient tome on %{skill}.\n \n" );
        const std::string & skill_name = Skill::Secondary::String( skill.Skill() );
        StringReplace( msg, "%{skill}", skill_name );

        const std::string title( MP2::StringObject( objectType ) );

        // check full
        if ( hero.HasMaxSecondarySkill() ) {
            msg.append( _(
                "As you approach, she turns and focuses her one glass eye on you.\n\"You already know everything you deserve to learn!\" the witch screeches. \"NOW GET OUT OF MY HOUSE!\"" ) );
            Dialog::Message( title, msg, Font::BIG, Dialog::OK );
        }
        else
            // check present skill
            if ( hero.HasSecondarySkill( skill.Skill() ) ) {
            msg.append( _( "As you approach, she turns and speaks.\n\"You already know that which I would teach you. I can help you no further.\"" ) );
            Dialog::Message( title, msg, Font::BIG, Dialog::OK );
        }
        else {
            hero.LearnSkill( skill );

            msg.append( _( "An ancient and immortal witch living in a hut with bird's legs for stilts teaches you %{skill} for her own inscrutable purposes." ) );
            StringReplace( msg, "%{skill}", skill_name );
            Dialog::SecondarySkillInfo( title, msg, skill, hero );
        }
    }

    hero.SetVisited( dst_index, Visit::GLOBAL );
    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToGoodLuckObject( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    bool visited = hero.isObjectTypeVisited( objectType );
    std::string msg;

    switch ( objectType ) {
    case MP2::OBJ_FOUNTAIN:
        msg = visited ? _( "You drink from the enchanted fountain, but nothing happens." ) : _( "As you drink the sweet water, you gain luck for your next battle." );
        break;

    case MP2::OBJ_FAERIERING:
        msg = visited ? _( "You enter the faerie ring, but nothing happens." ) : _( "Upon entering the mystical faerie ring, your army gains luck for its next battle." );
        break;

    case MP2::OBJ_IDOL:
        msg = visited ? _(
                  "You've found an ancient and weathered stone idol.\nIt is supposed to grant luck to visitors, but since the stars are already smiling upon you, it does nothing." )
                      : _( "You've found an ancient and weathered stone idol.\nKissing it is supposed to be lucky, so you do. The stone is very cold to the touch." );
        break;

    case MP2::OBJ_MERMAID:
        msg = visited
                  ? _( "The mermaids silently entice you to return later and be blessed again." )
                  : _(
                      "The magical, soothing beauty of the Mermaids reaches you and your crew.\nJust for a moment, you forget your worries and bask in the beauty of the moment.\nThe mermaids charms bless you with increased luck for your next combat." );
        break;

    default:
        break;
    }

    const std::string title( MP2::StringObject( objectType ) );

    // check already visited
    if ( visited ) {
        Dialog::Message( title, msg, Font::BIG, Dialog::OK );
    }
    else {
        // modify luck
        hero.SetVisited( dst_index );
        AGG::PlaySound( M82::GOODLUCK );
        DialogLuck( title, msg, true, 1 );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToPyramid( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    Maps::Tiles & tile = world.GetTiles( dst_index );
    const Spell & spell = tile.QuantitySpell();
    const std::string ask = _(
        "You come upon the pyramid of a great and ancient king.\nYou are tempted to search it for treasure, but all the old stories warn of fearful curses and undead "
        "guardians.\nWill you search?" );

    const std::string title( MP2::StringObject( objectType ) );

    if ( Dialog::YES == Dialog::Message( title, ask, Font::BIG, Dialog::YES | Dialog::NO ) ) {
        if ( spell.isValid() ) {
            // battle
            Army army( tile );

            const Battle::Result res = Battle::Loader( hero.GetArmy(), army, dst_index );
            if ( res.AttackerWins() ) {
                hero.IncreaseExperience( res.GetExperienceAttacker() );
                bool valid = false;

                std::string msg = _( "Upon defeating the monsters, you decipher an ancient glyph on the wall, telling the secret of the spell - '" );
                msg += spell.GetName();
                msg += "'.";

                // check magick book
                if ( !hero.HaveSpellBook() ) {
                    msg += '\n';
                    msg += _( "Unfortunately, you have no Magic Book to record the spell with." );
                }
                else if ( Skill::Level::EXPERT > hero.GetLevelSkill( Skill::Secondary::WISDOM ) ) {
                    // check skill level for wisdom
                    msg += '\n';
                    msg += _( "Unfortunately, you do not have the wisdom to understand the spell, and you are unable to learn it." );
                }
                else {
                    valid = true;
                }

                if ( valid ) {
                    Dialog::SpellInfo( title, msg, spell, true );
                    hero.AppendSpellToBook( spell );
                }
                else {
                    Dialog::Message( title, msg, Font::BIG, Dialog::OK );
                }

                tile.QuantityReset();
                hero.SetVisited( dst_index, Visit::GLOBAL );
            }
            else {
                BattleLose( hero, res, true );
            }
        }
        else {
            // modify luck
            AGG::PlaySound( M82::BADLUCK );
            const std::string msg = _( "You come upon the pyramid of a great and ancient king.\nRoutine exploration reveals that the pyramid is completely empty." );
            DialogLuck( title, msg, false, 2 );

            hero.SetVisited( dst_index, Visit::LOCAL );
            hero.SetVisited( dst_index, Visit::GLOBAL );
        }
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToSign( const Heroes & hero, s32 dst_index )
{
    const MapSign * sign = dynamic_cast<MapSign *>( world.GetMapObject( dst_index ) );
    Dialog::Message( MP2::StringObject( MP2::OBJ_SIGN ), ( sign ? sign->message : "" ), Font::BIG, Dialog::OK );

    (void)hero;
    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToMagicWell( Heroes & hero, int32_t dst_index )
{
    const uint32_t max = hero.GetMaxSpellPoints();
    const std::string title( MP2::StringObject( MP2::OBJ_MAGICWELL ) );

    if ( hero.GetSpellPoints() >= max ) {
        Dialog::Message( title, _( "A drink at the well is supposed to restore your spell points, but you are already at maximum." ), Font::BIG, Dialog::OK );
    }
    else
        // check already visited
        if ( hero.isObjectTypeVisited( MP2::OBJ_MAGICWELL ) ) {
        Dialog::Message( title, _( "A second drink at the well in one day will not help you." ), Font::BIG, Dialog::OK );
    }
    else {
        hero.SetVisited( dst_index );
        hero.SetSpellPoints( max );
        Dialog::Message( title, _( "A drink from the well has restored your spell points to maximum." ), Font::BIG, Dialog::OK );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToTradingPost( const Heroes & hero )
{
    Dialog::Marketplace( hero.GetKingdom(), true );

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToPrimarySkillObject( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    const Maps::Tiles & tile = world.GetTiles( dst_index );

    std::string msg;
    int skill = Skill::Primary::ATTACK;
    bool visited = hero.isVisited( tile );

    switch ( objectType ) {
    case MP2::OBJ_FORT:
        skill = Skill::Primary::DEFENSE;
        msg = visited ? _( "\"I'm sorry sir,\" The leader of the soldiers says, \"but you already know everything we have to teach.\"" )
                      : _( "The soldiers living in the fort teach you a few new defensive tricks." );
        break;

    case MP2::OBJ_MERCENARYCAMP:
        skill = Skill::Primary::ATTACK;
        msg = visited ? _(
                  "You've come upon a mercenary camp practicing their tactics. \"You're too advanced for us,\" the mercenary captain says. \"We can teach nothing more.\"" )
                      : _( "You've come upon a mercenary camp practicing their tactics. The mercenaries welcome you and your troops and invite you to train with them." );
        break;

    case MP2::OBJ_DOCTORHUT:
        skill = Skill::Primary::KNOWLEDGE;
        msg = visited
                  ? _( "\"Go 'way!\", the witch doctor barks, \"you know all I know.\"" )
                  : _(
                      "An Orcish witch doctor living in the hut deepens your knowledge of magic by showing you how to cast stones, read portents, and decipher the intricacies of chicken entrails." );
        break;

    case MP2::OBJ_STANDINGSTONES:
        skill = Skill::Primary::POWER;
        msg = visited ? _(
                  "You've found a group of Druids worshipping at one of their strange stone edifices. Silently, the Druids turn you away, indicating they have nothing new to teach you." )
                      : _( "You've found a group of Druids worshipping at one of their strange stone edifices. Silently, they teach you new ways to cast spells." );
        break;

    default:
        return;
    }

    const std::string title( MP2::StringObject( objectType ) );
    // check already visited
    if ( visited ) {
        Dialog::Message( title, msg, Font::BIG, Dialog::OK );
    }
    else {
        // increase skill
        hero.IncreasePrimarySkill( skill );
        hero.SetVisited( dst_index );
        Dialog::PrimarySkillInfo( title, msg, skill );

        // fix double action tile
        hero.SetVisitedWideTile( dst_index, objectType );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToPoorMoraleObject( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    Maps::Tiles & tile = world.GetTiles( dst_index );
    u32 gold = tile.QuantityGold();
    std::string ask;
    std::string msg;
    std::string win;

    switch ( objectType ) {
    case MP2::OBJ_GRAVEYARD:
        ask = _( "You tentatively approach the burial ground of ancient warriors. Do you want to search the graves?" );
        msg = _( "You spend several hours searching the graves and find nothing. Such a despicable act reduces your army's morale." );
        win = _( "Upon defeating the Zombies you search the graves and find something!" );
        break;
    case MP2::OBJ_SHIPWRECK:
        ask = _( "The rotting hulk of a great pirate ship creaks eerily as it is pushed against the rocks. Do you wish to search the shipwreck?" );
        msg = _( "You spend several hours sifting through the debris and find nothing. Such a despicable act reduces your army's morale." );
        win = _( "Upon defeating the Ghosts you sift through the debris and find something!" );
        break;
    case MP2::OBJ_DERELICTSHIP:
        ask = _( "The rotting hulk of a great pirate ship creaks eerily as it is pushed against the rocks. Do you wish to search the ship?" );
        msg = _( "You spend several hours sifting through the debris and find nothing. Such a despicable act reduces your army's morale." );
        win = _( "Upon defeating the Skeletons you sift through the debris and find something!" );
        break;
    default:
        break;
    }

    const std::string title( MP2::StringObject( objectType ) );

    if ( Dialog::YES == Dialog::Message( title, ask, Font::BIG, Dialog::YES | Dialog::NO ) ) {
        bool complete = false;

        if ( gold ) {
            Army army( tile );

            Battle::Result res = Battle::Loader( hero.GetArmy(), army, dst_index );
            if ( res.AttackerWins() ) {
                hero.IncreaseExperience( res.GetExperienceAttacker() );
                complete = true;
                const Artifact & art = tile.QuantityArtifact();

                if ( art.isValid() ) {
                    if ( hero.IsFullBagArtifacts() ) {
                        gold = GoldInsteadArtifact( objectType );
                        DialogWithGold( title, win, gold );
                    }
                    else {
                        DialogWithArtifactAndGold( title, win, art, gold );
                        hero.PickupArtifact( art );
                    }
                }
                else
                    DialogWithGold( title, win, gold );

                hero.GetKingdom().AddFundsResource( Funds( Resource::GOLD, gold ) );
            }
            else {
                BattleLose( hero, res, true );
            }
        }

        if ( complete ) {
            tile.QuantityReset();
            hero.SetVisited( dst_index, Visit::GLOBAL );
        }
        else if ( 0 == gold ) {
            // modify morale
            hero.SetVisited( dst_index, Visit::LOCAL );
            hero.SetVisited( dst_index, Visit::GLOBAL );
            AGG::PlaySound( M82::BADMRLE );
            DialogMorale( title, msg, false, 1 );
        }
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToGoodMoraleObject( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    std::string msg;
    u32 move = 0;
    bool visited = hero.isObjectTypeVisited( objectType );

    switch ( objectType ) {
    case MP2::OBJ_BUOY:
        msg = visited ? _( "Your men spot a navigational buoy, confirming that you are on course." )
                      : _( "Your men spot a navigational buoy, confirming that you are on course and increasing their morale." );
        break;

    case MP2::OBJ_OASIS:
        msg = visited ? _( "The drink at the oasis is refreshing, but offers no further benefit. The oasis might help again if you fought a battle first." )
                      : _( "A drink at the oasis fills your troops with strength and lifts their spirits.  You can travel a bit further today." );
        move = 800; // + 8TP, from FAQ
        break;

    case MP2::OBJ_WATERINGHOLE:
        msg = visited
                  ? _( "The drink at the watering hole is refreshing, but offers no further benefit. The watering hole might help again if you fought a battle first." )
                  : _( "A drink at the watering hole fills your troops with strength and lifts their spirits. You can travel a bit further today." );
        move = 400; // + 4TP, from FAQ
        break;

    case MP2::OBJ_TEMPLE:
        msg = visited ? _( "It doesn't help to pray twice before a battle. Come back after you've fought." )
                      : _( "A visit and a prayer at the temple raises the morale of your troops." );
        break;

    default:
        return;
    }

    const std::string title( MP2::StringObject( objectType ) );
    // check already visited
    if ( visited ) {
        Dialog::Message( title, msg, Font::BIG, Dialog::OK );
    }
    else {
        // modify morale
        hero.SetVisited( dst_index );
        AGG::PlaySound( M82::GOODMRLE );
        DialogMorale( title, msg, true, ( objectType == MP2::OBJ_TEMPLE ? 2 : 1 ) );
        hero.IncreaseMovePoints( move );

        // fix double action tile
        hero.SetVisitedWideTile( dst_index, objectType );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToExperienceObject( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    const Maps::Tiles & tile = world.GetTiles( dst_index );

    bool visited = hero.isVisited( tile );
    std::string msg;

    u32 exp = 0;

    switch ( objectType ) {
    case MP2::OBJ_GAZEBO:
        msg = visited ? _( "An old Knight appears on the steps of the gazebo. \"I am sorry, my liege, I have taught you all I can.\"" )
                      : _( "An old Knight appears on the steps of the gazebo. \"My liege, I will teach you all that I know to aid you in your travels.\"" );
        exp = 1000;
        break;

    default:
        return;
    }

    const std::string title( MP2::StringObject( objectType ) );
    // check already visited
    if ( visited ) {
        Dialog::Message( title, msg, Font::BIG, Dialog::OK );
    }
    else {
        if ( Settings::Get().MusicMIDI() ) {
            AGG::PlaySound( M82::EXPERNCE );
        }
        else {
            AGG::PlayMusic( MUS::EXPERIENCE, false );
        }
        DialogWithExp( title, msg, exp );

        // visit
        hero.SetVisited( dst_index );
        hero.IncreaseExperience( exp );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToShipwreckSurvivor( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    Maps::Tiles & tile = world.GetTiles( dst_index );

    const std::string title( MP2::StringObject( objectType ) );

    if ( hero.IsFullBagArtifacts() ) {
        const u32 gold = GoldInsteadArtifact( objectType );
        DialogWithGold(
            title,
            _( "You've pulled a shipwreck survivor from certain death in an unforgiving ocean. Grateful, he says, \"I would give you an artifact as a reward, but you're all full.\"" ),
            gold, Dialog::OK );
        hero.GetKingdom().AddFundsResource( Funds( Resource::GOLD, gold ) );
    }
    else {
        const Artifact & art = tile.QuantityArtifact();
        std::string str = _(
            "You've pulled a shipwreck survivor from certain death in an unforgiving ocean. Grateful, he rewards you for your act of kindness by giving you the %{art}." );
        StringReplace( str, "%{art}", art.GetName() );
        AGG::PlaySound( M82::TREASURE );
        Dialog::ArtifactInfo( title, str, art );
        hero.PickupArtifact( art );
    }

    Game::PlayPickupSound();

    Game::ObjectFadeAnimation::PrepareFadeTask( tile.GetObject(), tile.GetIndex(), -1, true, false );

    tile.RemoveObjectSprite();
    tile.QuantityReset();

    Game::ObjectFadeAnimation::PerformFadeTask();

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToArtifact( Heroes & hero, s32 dst_index )
{
    Maps::Tiles & tile = world.GetTiles( dst_index );
    const std::string title( MP2::StringObject( MP2::OBJ_ARTIFACT ) );

    if ( hero.IsFullBagArtifacts() )
        Dialog::Message( title, _( "You cannot pick up this artifact, you already have a full load!" ), Font::BIG, Dialog::OK );
    else {
        u32 cond = tile.QuantityVariant();
        Artifact art = tile.QuantityArtifact();

        bool result = false;
        std::string msg;

        // 1,2,3 - gold, gold + res
        if ( 0 < cond && cond < 4 ) {
            Funds payment = tile.QuantityFunds();

            if ( 1 == cond ) {
                msg = _( "A leprechaun offers you the %{art} for the small price of %{gold} Gold." );
                StringReplace( msg, "%{gold}", payment.Get( Resource::GOLD ) );
            }
            else {
                msg = _( "A leprechaun offers you the %{art} for the small price of %{gold} Gold and %{count} %{res}." );

                StringReplace( msg, "%{gold}", payment.Get( Resource::GOLD ) );
                ResourceCount rc = tile.QuantityResourceCount();
                StringReplace( msg, "%{count}", rc.second );
                StringReplace( msg, "%{res}", Resource::String( rc.first ) );
            }
            StringReplace( msg, "%{art}", art.GetName() );
            msg += '\n';
            msg.append( _( "Do you wish to buy this artifact?" ) );

            AGG::PlaySound( M82::EXPERNCE );
            if ( Dialog::YES == Dialog::ArtifactInfo( _( "Artifact" ), msg, art, Dialog::YES | Dialog::NO ) ) {
                if ( hero.GetKingdom().AllowPayment( payment ) ) {
                    result = true;
                    hero.GetKingdom().OddFundsResource( payment );
                }
                else {
                    Dialog::Message( title, _( "You try to pay the leprechaun, but realize that you can't afford it. The leprechaun stamps his foot and ignores you." ),
                                     Font::BIG, Dialog::OK );
                }
            }
            else {
                Dialog::Message( title, _( "Insulted by your refusal of his generous offer, the leprechaun stamps his foot and ignores you." ), Font::BIG, Dialog::OK );
            }
        }
        else
            // 4,5 - need have skill wisard or leadership,
            if ( 3 < cond && cond < 6 ) {
            const Skill::Secondary & skill = tile.QuantitySkill();

            if ( hero.HasSecondarySkill( skill.Skill() ) ) {
                msg = _( "You've found the artifact: " );
                msg.append( art.GetName() );
                AGG::PlaySound( M82::TREASURE );
                Dialog::ArtifactInfo( title, msg, art, Dialog::OK );
                result = true;
            }
            else {
                if ( skill.Skill() == Skill::Secondary::WISDOM ) {
                    msg = _(
                        "You've found the humble dwelling of a withered hermit. The hermit tells you that he is willing to give the %{art} to the first wise person he meets." );
                }
                else if ( skill.Skill() == Skill::Secondary::LEADERSHIP ) {
                    msg = _(
                        "You've come across the spartan quarters of a retired soldier. The soldier tells you that he is willing to pass on the %{art} to the first true leader he meets." );
                }
                else {
                    // Did you add a new condition? If yes add a proper if-else branch.
                    assert( 0 );
                    msg = _( "You've encountered a strange person with a hat and an owl on it. He tells is you that he is willing to give %{art} if you have %{skill}." );
                    StringReplace( msg, "%{skill}", skill.GetName() );
                }

                StringReplace( msg, "%{art}", art.GetName() );
                Dialog::Message( title, msg, Font::BIG, Dialog::OK );
            }
        }
        else
            // 6 - 50 rogues, 7 - 1 gin, 8,9,10,11,12,13 - 1 monster level4
            if ( 5 < cond && cond < 14 ) {
            bool battle = true;
            Army army( tile );
            const Troop * troop = army.GetFirstValid();

            if ( troop ) {
                if ( Monster::ROGUE == troop->GetID() )
                    Dialog::Message( title,
                                     _( "You come upon an ancient artifact. As you reach for it, a pack of Rogues leap out of the brush to guard their stolen loot." ),
                                     Font::BIG, Dialog::OK );
                else {
                    msg = _(
                        "Through a clearing you observe an ancient artifact. Unfortunately, it's guarded by a nearby %{monster}. Do you want to fight the %{monster} for the artifact?" );
                    StringReplace( msg, "%{monster}", troop->GetName() );
                    battle = ( Dialog::YES == Dialog::Message( title, msg, Font::BIG, Dialog::YES | Dialog::NO ) );
                }
            }

            if ( battle ) {
                // new battle
                Battle::Result res = Battle::Loader( hero.GetArmy(), army, dst_index );
                if ( res.AttackerWins() ) {
                    hero.IncreaseExperience( res.GetExperienceAttacker() );
                    result = true;
                    msg = _( "Victorious, you take your prize, the %{art}." );
                    StringReplace( msg, "%{art}", art.GetName() );
                    AGG::PlaySound( M82::TREASURE );
                    Dialog::ArtifactInfo( title, msg, art.GetID() );
                }
                else {
                    BattleLose( hero, res, true );
                }
            }
            else {
                Dialog::Message( title, _( "Discretion is the better part of valor, and you decide to avoid this fight for today." ), Font::BIG, Dialog::OK );
            }
        }
        else {
            if ( Artifact::GetScenario( art ) )
                msg = Artifact::GetScenario( art );
            else {
                msg = _( "You've found the artifact: " );
                msg += '\n';
                msg.append( art.GetName() );
            }
            AGG::PlaySound( M82::TREASURE );
            Dialog::ArtifactInfo( title, msg, art );
            result = true;
        }

        if ( result && hero.PickupArtifact( art ) ) {
            Game::PlayPickupSound();

            Game::ObjectFadeAnimation::PrepareFadeTask( tile.GetObject(), tile.GetIndex(), -1, true, false );

            tile.RemoveObjectSprite();
            tile.QuantityReset();

            Game::ObjectFadeAnimation::PerformFadeTask();
        }
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToTreasureChest( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    Maps::Tiles & tile = world.GetTiles( dst_index );
    const std::string & hdr = MP2::StringObject( objectType );

    std::string msg;
    u32 gold = tile.QuantityGold();

    // dialog
    if ( tile.isWater() ) {
        if ( gold ) {
            const Artifact & art = tile.QuantityArtifact();

            if ( art.isValid() ) {
                if ( hero.IsFullBagArtifacts() ) {
                    gold = GoldInsteadArtifact( objectType );
                    msg = _( "After spending hours trying to fish the chest out of the sea, you open it and find %{gold} gold pieces." );
                    StringReplace( msg, "%{gold}", gold );
                    DialogWithGold( hdr, msg, gold );
                }
                else {
                    msg = _( "After spending hours trying to fish the chest out of the sea, you open it and find %{gold} gold and the %{art}." );
                    StringReplace( msg, "%{gold}", gold );
                    StringReplace( msg, "%{art}", art.GetName() );
                    DialogWithArtifactAndGold( hdr, msg, art, gold );
                    hero.PickupArtifact( art );
                }
            }
            else {
                msg = _( "After spending hours trying to fish the chest out of the sea, you open it and find %{gold} gold pieces." );
                StringReplace( msg, "%{gold}", gold );
                DialogWithGold( hdr, msg, gold );
            }
        }
        else {
            Dialog::Message( hdr, _( "After spending hours trying to fish the chest out of the sea, you open it, only to find it empty." ), Font::BIG, Dialog::OK );
        }
    }
    else {
        const Artifact & art = tile.QuantityArtifact();

        if ( gold ) {
            const u32 expr = gold > 500 ? gold - 500 : 500;
            msg = _(
                "After scouring the area, you fall upon a hidden treasure cache. You may take the gold or distribute the gold to the peasants for experience. Do you wish to keep the gold?" );

            if ( !Dialog::SelectGoldOrExp( hdr, msg, gold, expr, hero ) ) {
                gold = 0;
                hero.IncreaseExperience( expr );
            }
        }
        else if ( art.isValid() ) {
            if ( hero.IsFullBagArtifacts() ) {
                gold = GoldInsteadArtifact( objectType );
                msg = _( "After scouring the area, you fall upon a hidden chest, containing the %{gold} gold pieces." );
                StringReplace( msg, "%{gold}", gold );
                DialogWithGold( hdr, msg, gold );
            }
            else {
                msg = _( "After scouring the area, you fall upon a hidden chest, containing the ancient artifact %{art}." );
                StringReplace( msg, "%{art}", art.GetName() );
                AGG::PlaySound( M82::TREASURE );
                Dialog::ArtifactInfo( hdr, msg, art );
                hero.PickupArtifact( art );
            }
        }
    }

    if ( gold )
        hero.GetKingdom().AddFundsResource( Funds( Resource::GOLD, gold ) );

    Game::PlayPickupSound();

    Game::ObjectFadeAnimation::PrepareFadeTask( tile.GetObject(), tile.GetIndex(), -1, true, false );

    tile.RemoveObjectSprite();
    tile.QuantityReset();

    Game::ObjectFadeAnimation::PerformFadeTask();

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToAncientLamp( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    Maps::Tiles & tile = world.GetTiles( dst_index );
    const Troop & troop = tile.QuantityTroop();
    if ( !troop.isValid() ) {
        return;
    }

    const std::string title( MP2::StringObject( objectType ) );
    if ( Dialog::YES
         == Dialog::Message( title, _( "You stumble upon a dented and tarnished lamp lodged deep in the earth. Do you wish to rub the lamp?" ), Font::BIG,
                             Dialog::YES | Dialog::NO ) ) {
        RecruitMonsterFromTile( hero, tile, title, troop, true );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToTeleports( Heroes & hero, s32 index_from )
{
    s32 index_to = world.NextTeleport( index_from );

    if ( index_from == index_to ) {
        AGG::PlaySound( M82::RSBRYFZL );
        DEBUG_LOG( DBG_GAME, DBG_WARN, "action unsuccessfully..." );
        return;
    }

    const Heroes * other_hero = world.GetTiles( index_to ).GetHeroes();
    if ( other_hero ) {
        ActionToHeroes( hero, index_to );

        // lose battle
        if ( hero.isFreeman() ) {
            return;
        }
        else if ( !other_hero->isFreeman() ) {
            DEBUG_LOG( DBG_GAME, DBG_WARN, "is busy..." );
            return;
        }
    }

    AGG::PlaySound( M82::KILLFADE );
    hero.GetPath().Hide();
    hero.FadeOut();

    hero.Move2Dest( index_to ); // no action and no penalty

    Interface::Basic & I = Interface::Basic::Get();
    I.GetGameArea().SetCenter( hero.GetCenter() );
    I.RedrawFocus();
    I.Redraw();

    AGG::PlaySound( M82::KILLFADE );
    hero.GetPath().Hide();
    hero.FadeIn();

    hero.GetPath().Reset();
    hero.GetPath().Show(); // Reset method sets Hero's path to hidden mode with non empty path, we have to set it back
    hero.ActionNewPosition( false );

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToWhirlpools( Heroes & hero, s32 index_from )
{
    const s32 index_to = world.NextWhirlpool( index_from );

    if ( index_from == index_to ) {
        AGG::PlaySound( M82::RSBRYFZL );
        DEBUG_LOG( DBG_GAME, DBG_WARN, "action unsuccessfully..." );
        return;
    }

    AGG::PlaySound( M82::KILLFADE );
    hero.GetPath().Hide();
    hero.FadeOut();

    hero.Move2Dest( index_to ); // no action and no penalty

    Interface::Basic & I = Interface::Basic::Get();
    I.GetGameArea().SetCenter( hero.GetCenter() );
    I.RedrawFocus();
    I.Redraw();

    AGG::PlaySound( M82::KILLFADE );
    hero.GetPath().Hide();
    hero.FadeIn();

    WhirlpoolTroopLooseEffect( hero );

    hero.GetPath().Reset();
    hero.GetPath().Show(); // Reset method sets Hero's path to hidden mode with non empty path, we have to set it back
    hero.ActionNewPosition( false );

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToAbandoneMine( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    if ( Dialog::YES
         == Dialog::Message( MP2::StringObject( MP2::OBJ_ABANDONEDMINE ),
                             _( "You come upon an abandoned gold mine. The mine appears to be haunted. Do you wish to enter?" ), Font::BIG, Dialog::YES | Dialog::NO ) ) {
        ActionToCaptureObject( hero, objectType, dst_index );
    }
}

/* capture color object */
void ActionToCaptureObject( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    Maps::Tiles & tile = world.GetTiles( dst_index );
    std::string header;
    std::string body;
    u32 resource = Resource::UNKNOWN;

    switch ( objectType ) {
    case MP2::OBJ_ALCHEMYLAB:
        resource = Resource::MERCURY;
        header = MP2::StringObject( objectType );
        body = _( "You have taken control of the local Alchemist shop. It will provide you with %{count} unit of Mercury per day." );
        break;

    case MP2::OBJ_SAWMILL:
        resource = Resource::WOOD;
        header = MP2::StringObject( objectType );
        body = _( "You gain control of a sawmill. It will provide you with %{count} units of wood per day." );
        break;

    case MP2::OBJ_ABANDONEDMINE:
    case MP2::OBJ_MINES: {
        if ( objectType == MP2::OBJ_ABANDONEDMINE && tile.GetQuantity3() != Spell::HAUNT ) {
            body = _( "You beat the Ghosts and are able to restore the mine to production." );
            break;
        }

        resource = tile.QuantityResourceCount().first;
        header = Maps::GetMinesName( resource );

        switch ( resource ) {
        case Resource::ORE:
            body = _( "You gain control of an ore mine. It will provide you with %{count} units of ore per day." );
            break;
        case Resource::SULFUR:
            body = _( "You gain control of a sulfur mine. It will provide you with %{count} unit of sulfur per day." );
            break;
        case Resource::CRYSTAL:
            body = _( "You gain control of a crystal mine. It will provide you with %{count} unit of crystal per day." );
            break;
        case Resource::GEMS:
            body = _( "You gain control of a gem mine. It will provide you with %{count} unit of gems per day." );
            break;
        case Resource::GOLD:
            body = _( "You gain control of a gold mine. It will provide you with %{count} gold per day." );
            break;
        default:
            break;
        }
        break;
    }

    case MP2::OBJ_LIGHTHOUSE:
        header = MP2::StringObject( objectType );
        body = _( "The lighthouse is now under your control, and all of your ships will now move further each turn." );
        break;

    default:
        body = _( "You gain control of a %{name}." );
        header = MP2::StringObject( objectType );
        StringReplace( body, "%{name}", MP2::StringObject( objectType ) );
        break;
    }

    // capture object
    if ( !hero.isFriends( tile.QuantityColor() ) ) {
        bool capture = true;

        // check guardians
        if ( tile.CaptureObjectIsProtection() ) {
            Army army( tile );
            const Monster & mons = tile.QuantityMonster();

            Battle::Result result = Battle::Loader( hero.GetArmy(), army, dst_index );

            if ( result.AttackerWins() ) {
                hero.IncreaseExperience( result.GetExperienceAttacker() );
                tile.SetQuantity3( 0 );
            }
            else {
                capture = false;
                BattleLose( hero, result, true );
                tile.MonsterSetCount( army.GetCountMonsters( mons ) );
            }
        }

        if ( capture ) {
            if ( resource == Resource::UNKNOWN )
                Dialog::Message( header, body, Font::BIG, Dialog::OK );
            else
                DialogCaptureResourceObject( header, body, resource );

            // update abandone mine
            if ( objectType == MP2::OBJ_ABANDONEDMINE ) {
                Maps::Tiles::UpdateAbandoneMineSprite( tile );
                hero.SetMapsObject( MP2::OBJ_MINES );
            }

            tile.QuantitySetColor( hero.GetColor() );
        }
    }
    else
        // set guardians
        if ( Settings::Get().ExtWorldAllowSetGuardian() ) {
        CapturedObject & co = world.GetCapturedObject( dst_index );
        Troop & troop1 = co.GetTroop();
        Troop troop2 = troop1;

        // check if it is already guarded by a spell
        const bool readonly = tile.GetQuantity3() != 0;

        if ( Dialog::SetGuardian( hero, troop2, co, readonly ) )
            troop1.Set( troop2.GetMonster(), troop2.GetCount() );
    }

    if ( objectType == MP2::OBJ_LIGHTHOUSE )
        world.CaptureObject( dst_index, hero.GetColor() );

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() << " captured: " << MP2::StringObject( objectType ) );
}

void ActionToDwellingJoinMonster( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    Maps::Tiles & tile = world.GetTiles( dst_index );
    const Troop & troop = tile.QuantityTroop();

    const std::string title( MP2::StringObject( objectType ) );

    if ( troop.isValid() ) {
        hero.MovePointsScaleFixed();

        std::string message = _( "A group of %{monster} with a desire for greater glory wish to join you. Do you accept?" );
        StringReplace( message, "%{monster}", troop.GetMultiName() );

        if ( !Settings::Get().MusicMIDI() && objectType == MP2::OBJ_WATCHTOWER )
            AGG::PlayMusic( MUS::WATCHTOWER, false );
        else
            AGG::PlaySound( M82::EXPERNCE );

        if ( Dialog::YES == Dialog::Message( title, message, Font::BIG, Dialog::YES | Dialog::NO ) ) {
            if ( !hero.GetArmy().CanJoinTroop( troop ) )
                Dialog::Message( troop.GetName(), _( "You are unable to recruit at this time, your ranks are full." ), Font::BIG, Dialog::OK );
            else {
                tile.MonsterSetCount( 0 );
                hero.GetArmy().JoinTroop( troop );
                hero.MovePointsScaleFixed();

                Interface::Basic::Get().GetStatusWindow().SetRedraw();
            }
        }
    }
    else {
        Dialog::Message( title, _( "As you approach the dwelling, you notice that there is no one here." ), Font::BIG, Dialog::OK );
    }

    hero.SetVisited( dst_index, Visit::GLOBAL );

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() << ", object: " << title.c_str() );
}

void ActionToDwellingRecruitMonster( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    Maps::Tiles & tile = world.GetTiles( dst_index );

    std::string msg_full;
    std::string msg_void;

    switch ( objectType ) {
    case MP2::OBJ_RUINS:
        msg_void = _( "You search the ruins, but the Medusas that used to live here are gone. Perhaps there will be more next week." );
        msg_full = _( "You've found some Medusas living in the ruins. They are willing to join your army for a price. Do you want to recruit Medusas?" );
        break;

    case MP2::OBJ_TREECITY:
        msg_void = _( "You've found a Sprite Tree City. Unfortunately, none of the Sprites living there wish to join an army. Maybe next week." );
        msg_full = _( "Some of the Sprites living in the tree city are willing to join your army for a price. Do you want to recruit Sprites?" );
        break;

    case MP2::OBJ_WAGONCAMP:
        msg_void = _( "A colorful Rogues' wagon stands empty here. Perhaps more Rogues will be here later." );
        msg_full = _( "Distant sounds of music and laughter draw you to a colorful wagon housing Rogues. Do you wish to have any Rogues join your army?" );
        break;

    case MP2::OBJ_DESERTTENT:
        msg_void = _( "A group of tattered tents, billowing in the sandy wind, beckons you. The tents are unoccupied. Perhaps more Nomads will be here later." );
        msg_full = _( "A group of tattered tents, billowing in the sandy wind, beckons you. Do you wish to have any Nomads join you during your travels?" );
        break;

    case MP2::OBJ_EARTHALTAR:
        msg_void = _( "The pit of mud bubbles for a minute and then lies still." );
        msg_full = _(
            "As you approach the bubbling pit of mud, creatures begin to climb out and position themselves around it. In unison they say: \"Mother Earth would like to offer you a few of her troops. Do you want to recruit Earth Elementals?\"" );
        break;

    case MP2::OBJ_AIRALTAR:
        msg_void = _( "You enter the structure of white stone pillars, and find nothing." );
        msg_full = _(
            "White stone pillars support a roof that rises up to the sky. As you enter the structure, the dead air of the outside gives way to a whirling gust that almost pushes you back out. The air current materializes into a barely visible form. The creature asks, in what can only be described as a loud whisper: \"Why have you come? Are you here to call upon the forces of the air?\"" );
        break;

    case MP2::OBJ_FIREALTAR:
        msg_void = _( "No Fire Elementals approach you from the lava pool." );
        msg_full = _(
            "Beneath a structure that serves to hold in heat, Fire Elementals move about in a fiery pool of molten lava. A group of them approach you and offer their services. Would you like to recruit Fire Elementals?" );
        break;

    case MP2::OBJ_WATERALTAR:
        msg_void = _( "A face forms in the water for a moment, and then is gone." );
        msg_full = _(
            "Crystalline structures cast shadows over a small reflective pool of water. You peer into the pool, and a face that is not your own peers back. It asks: \"Would you like to call upon the powers of water?\"" );
        break;

    case MP2::OBJ_BARROWMOUNDS:
        msg_void = _( "This burial site is deathly still." );
        msg_full
            = _( "Restless spirits of long dead warriors seeking their final resting place offer to join you in hopes of finding peace. Do you wish to recruit ghosts?" );
        break;

    default:
        return;
    }
    AGG::PlayMusic( MUS::FromMapObject( objectType ), false );

    const Troop & troop = tile.QuantityTroop();

    const std::string title( MP2::StringObject( objectType ) );

    if ( !troop.isValid() )
        Dialog::Message( title, msg_void, Font::BIG, Dialog::OK );
    else if ( Dialog::YES == Dialog::Message( title, msg_full, Font::BIG, Dialog::YES | Dialog::NO ) )
        RecruitMonsterFromTile( hero, tile, title, troop, false );

    hero.SetVisited( dst_index, Visit::GLOBAL );

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() << ", object: " << title.c_str() );
}

void ActionToDwellingBattleMonster( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    const char * str_empty = nullptr;
    const char * str_recr = nullptr;
    const char * str_warn = nullptr;
    const char * str_wins = nullptr;
    const char * str_scss = nullptr;

    switch ( objectType ) {
    case MP2::OBJ_CITYDEAD:
        str_empty = _( "The City of the Dead is empty of life, and empty of unlife as well. Perhaps some undead will move in next week." );
        str_recr = _( "Some Liches living here are willing to join your army for a price. Do you want to recruit Liches?" );
        str_warn = _( "You've found the ruins of an ancient city, now inhabited solely by the undead. Will you search?" );
        str_wins
            = _( "Some of the surviving Liches are impressed by your victory over their fellows, and offer to join you for a price. Do you want to recruit Liches?" );
        break;
    case MP2::OBJ_TROLLBRIDGE:
        str_empty = _( "You've found one of those bridges that Trolls are so fond of living under, but there are none here. Perhaps there will be some next week." );
        str_recr = _( "Some Trolls living under a bridge are willing to join your army, but for a price. Do you want to recruit Trolls?" );
        str_warn = _( "Trolls living under the bridge challenge you. Will you fight them?" );
        str_wins = _( "A few Trolls remain, cowering under the bridge. They approach you and offer to join your forces as mercenaries. Do you want to buy any Trolls?" );
        break;
    case MP2::OBJ_DRAGONCITY:
        str_empty = _( "The Dragon city has no Dragons willing to join you this week. Perhaps a Dragon will become available next week." );
        str_recr = _( "The Dragon city is willing to offer some Dragons for your army for a price. Do you wish to recruit Dragons?" );
        str_warn = _( "You stand before the Dragon City, a place off-limits to mere humans. Do you wish to violate this rule and challenge the Dragons to a fight?" );
        str_wins = _( "Having defeated the Dragon champions, the city's leaders agree to supply some Dragons to your army for a price. Do you wish to recruit Dragons?" );
        break;
    default:
        return;
    }

    Maps::Tiles & tile = world.GetTiles( dst_index );
    const Troop & troop = tile.QuantityTroop();

    const std::string title( MP2::StringObject( objectType ) );

    if ( Color::NONE == tile.QuantityColor() ) {
        // Not captured / defeated yet.
        if ( Dialog::YES == Dialog::Message( title, str_warn, Font::BIG, Dialog::YES | Dialog::NO ) ) {
            // new battle
            Army army( tile );
            Battle::Result res = Battle::Loader( hero.GetArmy(), army, dst_index );
            if ( res.AttackerWins() ) {
                hero.IncreaseExperience( res.GetExperienceAttacker() );
                tile.QuantitySetColor( hero.GetColor() );
                tile.SetObjectPassable( true );
                str_scss = str_wins;
            }
            else {
                BattleLose( hero, res, true );
            }
        }
    }
    else {
        if ( troop.isValid() ) {
            str_scss = str_recr;
        }
        else {
            Dialog::Message( title, str_empty, Font::BIG, Dialog::OK );
        }
    }

    // recruit monster
    if ( str_scss ) {
        if ( troop.isValid() && Dialog::YES == Dialog::Message( title, str_scss, Font::BIG, Dialog::YES | Dialog::NO ) ) {
            RecruitMonsterFromTile( hero, tile, title, troop, false );
        }

        hero.SetVisited( dst_index, Visit::GLOBAL );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() << ", object: " << title.c_str() );
}

void ActionToObservationTower( const Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    if ( !Settings::Get().MusicMIDI() ) {
        AGG::PlayMusic( MUS::WATCHTOWER, true );
    }

    Dialog::Message( MP2::StringObject( objectType ), _( "From the observation tower, you are able to see distant lands." ), Font::BIG, Dialog::OK );

    Maps::ClearFog( dst_index, Game::GetViewDistance( Game::VIEW_OBSERVATION_TOWER ), hero.GetColor() );
}

void ActionToArtesianSpring( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    const u32 max = hero.GetMaxSpellPoints();
    const std::string & name = MP2::StringObject( MP2::OBJ_ARTESIANSPRING );

    if ( world.isAnyKingdomVisited( objectType, dst_index ) ) {
        Dialog::Message( name, _( "The spring only refills once a week, and someone's already been here this week." ), Font::BIG, Dialog::OK );
    }
    else if ( hero.GetSpellPoints() == max * 2 ) {
        Dialog::Message( name, _( "A drink at the spring is supposed to give you twice your normal spell points, but you are already at that level." ), Font::BIG,
                         Dialog::OK );
    }
    else {
        if ( Settings::Get().MusicMIDI() ) {
            AGG::PlaySound( M82::EXPERNCE );
        }
        else {
            AGG::PlayMusic( MUS::WATERSPRING, false );
        }
        hero.SetSpellPoints( max * 2 );
        Dialog::Message( name, _( "A drink from the spring fills your blood with magic! You have twice your normal spell points in reserve." ), Font::BIG, Dialog::OK );
    }

    hero.SetVisitedWideTile( dst_index, objectType, Visit::GLOBAL );

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToXanadu( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    const Maps::Tiles & tile = world.GetTiles( dst_index );
    const std::string title( MP2::StringObject( objectType ) );

    if ( hero.isVisited( tile ) ) {
        Dialog::Message( title, _( "Recognizing you, the butler refuses to admit you. \"The master,\" he says, \"will not see the same student twice.\"" ), Font::BIG,
                         Dialog::OK );
    }
    else {
        bool access = false;
        switch ( hero.GetLevelSkill( Skill::Secondary::DIPLOMACY ) ) {
        case Skill::Level::BASIC:
            if ( 7 < hero.GetLevel() )
                access = true;
            break;
        case Skill::Level::ADVANCED:
            if ( 5 < hero.GetLevel() )
                access = true;
            break;
        case Skill::Level::EXPERT:
            if ( 3 < hero.GetLevel() )
                access = true;
            break;
        default:
            if ( 9 < hero.GetLevel() )
                access = true;
            break;
        }

        if ( access ) {
            Dialog::Message( title, _( "The butler admits you to see the master of the house. He trains you in the four skills a hero should know." ), Font::BIG,
                             Dialog::OK );
            hero.IncreasePrimarySkill( Skill::Primary::ATTACK );
            hero.IncreasePrimarySkill( Skill::Primary::DEFENSE );
            hero.IncreasePrimarySkill( Skill::Primary::KNOWLEDGE );
            hero.IncreasePrimarySkill( Skill::Primary::POWER );
            hero.SetVisited( dst_index );
        }
        else {
            Dialog::Message(
                title,
                _( "The butler opens the door and looks you up and down. \"You are neither famous nor diplomatic enough to be admitted to see my master,\" he sniffs. \"Come back when you think yourself worthy.\"" ),
                Font::BIG, Dialog::OK );
        }
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

bool ActionToUpgradeArmy( Army & army, const Monster & mons, std::string & str1, std::string & str2, const bool combineWithAnd )
{
    const std::string combTypeAnd = _( " and " );
    const std::string combTypeComma = ", ";

    if ( army.HasMonster( mons ) ) {
        army.UpgradeMonsters( mons );
        if ( !str1.empty() )
            str1 += combineWithAnd ? combTypeAnd : combTypeComma;
        str1 += mons.GetMultiName();
        if ( !str2.empty() )
            str2 += combineWithAnd ? combTypeAnd : combTypeComma;
        str2 += mons.GetUpgrade().GetMultiName();
        return true;
    }
    return false;
}

void ActionToUpgradeArmyObject( Heroes & hero, const MP2::MapObjectType objectType, const std::string & defaultMessage )
{
    std::string monsters;
    std::string monsters_upgrade;
    std::string msg1;
    std::string msg2;

    std::vector<Monster *> mons;

    hero.MovePointsScaleFixed();

    std::vector<Monster> monsToUpgrade;

    switch ( objectType ) {
    case MP2::OBJ_HILLFORT: {
        monsToUpgrade = { Monster( Monster::OGRE ), Monster( Monster::ORC ), Monster( Monster::DWARF ) };

        msg1 = _( "All of the %{monsters} you have in your army have been trained by the battle masters of the fort. Your army now contains %{monsters2}." );
        msg2 = _( "An unusual alliance of Ogres, Orcs, and Dwarves offer to train (upgrade) any such troops brought to them. Unfortunately, you have none with you." );
        break;
    }

    case MP2::OBJ_FREEMANFOUNDRY: {
        monsToUpgrade = { Monster( Monster::SWORDSMAN ), Monster( Monster::PIKEMAN ), Monster( Monster::IRON_GOLEM ) };

        msg1 = _( "All of your %{monsters} have been upgraded into %{monsters2}." );
        msg2 = _(
            "A blacksmith working at the foundry offers to convert all Pikemen and Swordsmen's weapons brought to him from iron to steel. He also says that he knows a process that will convert Iron Golems into Steel Golems. Unfortunately, you have none of these troops in your army, so he can't help you." );
        break;
    }
    case MP2::OBJ_STABLES: {
        assert( !defaultMessage.empty() );
        msg1 = defaultMessage;
        msg2 = defaultMessage;

        monsToUpgrade = { Monster( Monster::CAVALRY ) };
        break;
    }

    default:
        ERROR_LOG( "Incorrect object type passed to ActionToUpgradeArmyObject" );
        assert( 0 );
        return;
    }

    if ( monsToUpgrade.empty() ) {
        ERROR_LOG( "monsToUpgrade mustn't be empty." );
        assert( 0 );
        return;
    }

    Army & heroArmy = hero.GetArmy();
    mons.reserve( monsToUpgrade.size() );

    for ( size_t i = 0; i < monsToUpgrade.size(); ++i ) {
        if ( !heroArmy.HasMonster( monsToUpgrade[i] ) )
            continue;
        const bool combineWithAnd = i == ( monsToUpgrade.size() - 1 ) && !mons.empty();
        if ( ActionToUpgradeArmy( heroArmy, monsToUpgrade[i], monsters, monsters_upgrade, combineWithAnd ) )
            mons.emplace_back( &monsToUpgrade[i] );
    }

    const std::string title( MP2::StringObject( objectType ) );

    if ( !mons.empty() ) {
        // composite sprite
        uint32_t offsetX = 0;
        uint32_t offsetY = 0;
        const fheroes2::Sprite & border = fheroes2::AGG::GetICN( ICN::STRIP, 12 );

        const int32_t monsterCount = static_cast<int32_t>( mons.size() ); // safe to do as the count is no more than 3

        const int32_t monsterPerX = monsterCount > 1 ? 2 : 1;
        const int32_t monsterPerY = monsterCount == 1 ? 1 : ( monsterCount + ( monsterCount - 1 ) ) / 2;

        fheroes2::Image surface( border.width() * monsterPerX + ( monsterPerX - 1 ) * 4, border.height() * monsterPerY + ( monsterPerY - 1 ) * 4 );
        surface.reset();

        StringReplace( msg1, "%{monsters}", monsters );
        StringReplace( msg1, "%{monsters2}", monsters_upgrade );

        for ( size_t i = 0; i < mons.size(); ++i ) {
            if ( i > 0 && ( i & 1 ) == 0 ) {
                if ( i == mons.size() - 1 ) {
                    offsetX = border.width() / 2 + 2;
                }
                else {
                    offsetX = 0;
                }
                offsetY += border.height() + 4;
            }

            // border
            fheroes2::Blit( border, surface, offsetX, offsetY );
            // background scenary for each race
            switch ( mons[i]->GetRace() ) {
            case Race::KNGT:
                fheroes2::Blit( fheroes2::AGG::GetICN( ICN::STRIP, 4 ), surface, offsetX + 6, offsetY + 6 );
                break;
            case Race::BARB:
                fheroes2::Blit( fheroes2::AGG::GetICN( ICN::STRIP, 5 ), surface, offsetX + 6, offsetY + 6 );
                break;
            case Race::SORC:
                fheroes2::Blit( fheroes2::AGG::GetICN( ICN::STRIP, 6 ), surface, offsetX + 6, offsetY + 6 );
                break;
            case Race::WRLK:
                fheroes2::Blit( fheroes2::AGG::GetICN( ICN::STRIP, 7 ), surface, offsetX + 6, offsetY + 6 );
                break;
            case Race::WZRD:
                fheroes2::Blit( fheroes2::AGG::GetICN( ICN::STRIP, 8 ), surface, offsetX + 6, offsetY + 6 );
                break;
            case Race::NECR:
                fheroes2::Blit( fheroes2::AGG::GetICN( ICN::STRIP, 9 ), surface, offsetX + 6, offsetY + 6 );
                break;
            default:
                fheroes2::Blit( fheroes2::AGG::GetICN( ICN::STRIP, 10 ), surface, offsetX + 6, offsetY + 6 );
                break;
            }
            // upgraded troop
            const fheroes2::Sprite & mon = fheroes2::AGG::GetICN( mons[i]->GetUpgrade().ICNMonh(), 0 );

            fheroes2::Blit( mon, surface, offsetX + 6 + mon.x(), 6 + mon.y() + offsetY );
            offsetX += border.width() + 4;
        }
        Dialog::SpriteInfo( title, msg1, surface );
    }
    else {
        Dialog::Message( title, msg2, Font::BIG, Dialog::OK );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToMagellanMaps( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    const Funds payment( Resource::GOLD, 1000 );
    Kingdom & kingdom = hero.GetKingdom();

    const std::string title( MP2::StringObject( objectType ) );

    if ( hero.isObjectTypeVisited( objectType, Visit::GLOBAL ) ) {
        Dialog::Message( title, _( "The captain looks at you with surprise and says:\n\"You already have all the maps I know about. Let me fish in peace now.\"" ),
                         Font::BIG, Dialog::OK );
    }
    else if ( kingdom.AllowPayment( payment ) ) {
        if (
            Dialog::YES
            == Dialog::Message(
                title,
                _( "A retired captain living on this refurbished fishing platform offers to sell you maps of the sea he made in his younger days for 1,000 gold. Do you wish to buy the maps?" ),
                Font::BIG, Dialog::YES | Dialog::NO ) ) {
            world.ActionForMagellanMaps( hero.GetColor() );
            kingdom.OddFundsResource( payment );
            hero.SetVisited( dst_index, Visit::GLOBAL );
            hero.setVisitedForAllies( dst_index );
        }

        Interface::Basic & I = Interface::Basic::Get();
        I.RedrawFocus();
    }
    else {
        Dialog::Message( title, _( "The captain sighs. \"You don't have enough money, eh?  You can't expect me to give my maps away for free!\"" ), Font::BIG,
                         Dialog::OK );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToEvent( Heroes & hero, s32 dst_index )
{
    // check event maps
    MapEvent * event_maps = world.GetMapEvent( Maps::GetPoint( dst_index ) );

    if ( event_maps && event_maps->isAllow( hero.GetColor() ) ) {
        hero.SetMove( false );

        if ( event_maps->resources.GetValidItemsCount() ) {
            hero.GetKingdom().AddFundsResource( event_maps->resources );
            Dialog::ResourceInfo( "", event_maps->message, event_maps->resources );
        }
        else if ( !event_maps->message.empty() )
            Dialog::Message( "", event_maps->message, Font::BIG, Dialog::OK );

        const Artifact & art = event_maps->artifact;
        if ( art.isValid() ) {
            if ( hero.PickupArtifact( art ) ) {
                std::string message( _( "You find %{artifact}." ) );
                StringReplace( message, "%{artifact}", art.GetName() );
                AGG::PlaySound( M82::TREASURE );
                Dialog::ArtifactInfo( "", message, art );
                Game::PlayPickupSound();
            }
        }

        event_maps->SetVisited( hero.GetColor() );

        if ( event_maps->cancel ) {
            hero.SetMapsObject( MP2::OBJ_ZERO );
            world.RemoveMapObject( event_maps );
        }
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToObelisk( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    Kingdom & kingdom = hero.GetKingdom();
    const std::string title( MP2::StringObject( objectType ) );

    if ( !hero.isVisited( world.GetTiles( dst_index ), Visit::GLOBAL ) ) {
        hero.SetVisited( dst_index, Visit::GLOBAL );
        kingdom.PuzzleMaps().Update( kingdom.CountVisitedObjects( MP2::OBJ_OBELISK ), world.CountObeliskOnMaps() );
        AGG::PlaySound( M82::EXPERNCE );
        Dialog::Message(
            title,
            _( "You come upon an obelisk made from a type of stone you have never seen before. Staring at it intensely, the smooth surface suddenly changes to an inscription. The inscription is a piece of a lost ancient map. Quickly you copy down the piece and the inscription vanishes as abruptly as it appeared." ),
            Font::BIG, Dialog::OK );
        kingdom.PuzzleMaps().ShowMapsDialog();
    }
    else {
        Dialog::Message( title, _( "You have already been to this obelisk." ), Font::BIG, Dialog::OK );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToTreeKnowledge( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    const Maps::Tiles & tile = world.GetTiles( dst_index );
    const std::string title( MP2::StringObject( objectType ) );

    if ( hero.isVisited( tile ) ) {
        Dialog::Message( title,
                         _( "Upon your approach, the tree opens its eyes in delight. \"It is good to see you, my student. I hope my teachings have helped you.\"" ),
                         Font::BIG, Dialog::OK );
    }
    else {
        const Funds & funds = tile.QuantityFunds();
        bool conditions = 0 == funds.GetValidItemsCount();
        std::string msg;

        // free
        if ( conditions ) {
            const fheroes2::Sprite & sprite = fheroes2::AGG::GetICN( ICN::EXPMRL, 4 );
            msg = _(
                "Upon your approach, the tree opens its eyes in delight. \"Ahh, an adventurer! Allow me to teach you a little of what I have learned over the ages.\"" );
            Dialog::SpriteInfo( title, msg, sprite );
        }
        else {
            const ResourceCount & rc = tile.QuantityResourceCount();

            if ( hero.GetKingdom().AllowPayment( funds ) ) {
                msg = _( "Upon your approach, the tree opens its eyes in delight." );
                msg += '\n';
                msg.append( _( "\"Ahh, an adventurer! I will be happy to teach you a little of what I have learned over the ages for a mere %{count} %{res}.\"" ) );
                msg += '\n';
                msg.append( _( "(Just bury it around my roots.)" ) );
                StringReplace( msg, "%{res}", Resource::String( rc.first ) );
                StringReplace( msg, "%{count}", rc.second );
                conditions = Dialog::YES == Dialog::SpriteInfo( title, msg, fheroes2::AGG::GetICN( ICN::EXPMRL, 4 ), Dialog::YES | Dialog::NO );
            }
            else {
                msg = _( "Tears brim in the eyes of the tree." );
                msg += '\n';
                msg.append( _( "\"I need %{count} %{res}.\"" ) );
                msg += '\n';
                msg.append( _( "it whispers. (sniff) \"Well, come back when you can pay me.\"" ) );
                StringReplace( msg, "%{res}", Resource::String( rc.first ) );
                StringReplace( msg, "%{count}", rc.second );
                Dialog::Message( title, msg, Font::BIG, Dialog::OK );
            }
        }

        if ( conditions ) {
            hero.GetKingdom().OddFundsResource( funds );
            hero.SetVisited( dst_index );
            hero.IncreaseExperience( Heroes::GetExperienceFromLevel( hero.GetLevel() ) - hero.GetExperience() );
        }
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToOracle( const Heroes & hero, const MP2::MapObjectType objectType )
{
    Dialog::Message(
        MP2::StringObject( objectType ),
        _( "Nestled among the trees sits a blind seer. After explaining the intent of your journey, the seer activates his crystal ball, allowing you to see the strengths and weaknesses of your opponents." ),
        Font::BIG, Dialog::OK );

    Dialog::ThievesGuild( true );

    (void)hero;
    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToDaemonCave( Heroes & hero, const MP2::MapObjectType objectType, int32_t dst_index )
{
    Maps::Tiles & tile = world.GetTiles( dst_index );

    AGG::PlayMusic( MUS::DEMONCAVE, false );

    const std::string header = MP2::StringObject( objectType );
    if ( Dialog::YES
         == Dialog::Message( header, _( "The entrance to the cave is dark, and a foul, sulfurous smell issues from the cave mouth. Will you enter?" ), Font::BIG,
                             Dialog::YES | Dialog::NO ) ) {
        uint32_t variant = tile.QuantityVariant();

        if ( variant ) {
            uint32_t gold = tile.QuantityGold();
            std::string msg;

            if ( variant == 3 && hero.IsFullBagArtifacts() )
                variant = 2;

            if (
                Dialog::YES
                == Dialog::Message(
                    header,
                    _( "You find a powerful and grotesque Demon in the cave. \"Today,\" it rasps, \"you will fight and surely die. But I will give you a choice of deaths. You may fight me, or you may fight my servants. Do you prefer to fight my servants?\"" ),
                    Font::BIG, Dialog::YES | Dialog::NO ) ) {
                // battle with earth elements
                Army army( tile );
                gold = 2500;

                Battle::Result res = Battle::Loader( hero.GetArmy(), army, dst_index );
                if ( res.AttackerWins() ) {
                    hero.IncreaseExperience( res.GetExperienceAttacker() );
                    msg = _( "Upon defeating the daemon's servants, you find a hidden cache with %{count} gold." );
                    StringReplace( msg, "%{count}", gold );
                    DialogWithGold( header, msg, gold );
                    hero.GetKingdom().AddFundsResource( Funds( Resource::GOLD, gold ) );
                }
                else {
                    BattleLose( hero, res, true );
                }
            }
            // check variants
            else if ( 1 == variant ) {
                const uint32_t exp = 1000;
                msg = _( "The Demon screams its challenge and attacks! After a short, desperate battle, you slay the monster and receive %{exp} experience points." );
                StringReplace( msg, "%{exp}", exp );
                DialogWithExp( header, msg, exp );
                hero.IncreaseExperience( exp );
            }
            else if ( 2 == variant ) {
                const uint32_t exp = 1000;
                msg = _(
                    "The Demon screams its challenge and attacks! After a short, desperate battle, you slay the monster and receive %{exp} experience points and %{count} gold." );
                StringReplace( msg, "%{exp}", exp );
                StringReplace( msg, "%{count}", gold );
                DialogGoldWithExp( header, msg, gold, exp );
                hero.IncreaseExperience( exp );
                hero.GetKingdom().AddFundsResource( Funds( Resource::GOLD, gold ) );
            }
            else if ( 3 == variant ) {
                const uint32_t exp = 1000;
                const Artifact & art = tile.QuantityArtifact();
                msg = _(
                    "The Demon screams its challenge and attacks! After a short, desperate battle, you slay the monster and find the %{art} in the back of the cave." );
                StringReplace( msg, "%{art}", art.GetName() );
                if ( art.isValid() )
                    DialogArtifactWithExp( header, msg, art, exp );
                hero.PickupArtifact( art );
                hero.IncreaseExperience( exp );
            }
            else {
                bool remove = true;
                Funds payment( Resource::GOLD, gold );
                Kingdom & kingdom = hero.GetKingdom();
                bool allow = kingdom.AllowPayment( payment );

                msg = allow ? _(
                          "The Demon leaps upon you and has its claws at your throat before you can even draw your sword. \"Your life is mine,\" it says. \"I will sell it back to you for %{count} gold.\"" )
                            : _( "Seeing that you do not have %{count} gold, the demon slashes you with its claws, and the last thing you see is a red haze." );
                StringReplace( msg, "%{count}", gold );

                if ( allow ) {
                    if ( Dialog::YES == Dialog::Message( header, msg, Font::BIG, Dialog::YES | Dialog::NO ) ) {
                        remove = false;
                        kingdom.OddFundsResource( payment );
                    }
                }
                else
                    Dialog::Message( header, msg, Font::BIG, Dialog::OK );

                if ( remove ) {
                    Battle::Result res;
                    res.army1 = Battle::RESULT_LOSS;
                    BattleLose( hero, res, true );
                }
            }

            tile.QuantityReset();
        }
        else
            Dialog::Message( header, _( "Except for evidence of a terrible battle, the cave is empty." ), Font::BIG, Dialog::OK );

        hero.SetVisited( dst_index, Visit::GLOBAL );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToAlchemistsTower( Heroes & hero )
{
    BagArtifacts & bag = hero.GetBagArtifacts();
    const uint32_t cursed = static_cast<uint32_t>( std::count_if( bag.begin(), bag.end(), []( const Artifact & art ) { return art.isAlchemistRemove(); } ) );

    const char * title = MP2::StringObject( MP2::OBJ_ALCHEMYTOWER );

    if ( cursed ) {
        const payment_t payment = PaymentConditions::ForAlchemist();

        if ( hero.GetKingdom().AllowPayment( payment ) ) {
            std::string msg = _( "As you enter the Alchemist's Tower, a hobbled, graying man in a brown cloak makes his way towards you." );
            msg += '\n';
            msg.append(
                _n( "He checks your pack, and sees that you have 1 cursed item.", "He checks your pack, and sees that you have %{count} cursed items.", cursed ) );
            StringReplace( msg, "%{count}", cursed );
            msg += '\n';
            msg.append( _n( "For %{gold} gold, the alchemist will remove it for you. Do you pay?",
                            "For %{gold} gold, the alchemist will remove them for you. Do you pay?", cursed ) );
            StringReplace( msg, "%{gold}", payment.gold );

            if ( Dialog::YES == Dialog::Message( title, msg, Font::BIG, Dialog::YES | Dialog::NO ) ) {
                AGG::PlaySound( M82::GOODLUCK );
                hero.GetKingdom().OddFundsResource( payment );

                for ( Artifact & artifact : bag ) {
                    if ( artifact.isAlchemistRemove() ) {
                        artifact = Artifact::UNKNOWN;
                    }
                }

                msg = _n( "After you consent to pay the requested amount of gold, the alchemist grabs the cursed artifact and throws it into his magical cauldron.",
                          "After you consent to pay the requested amount of gold, the alchemist grabs all cursed artifacts and throws them into his magical cauldron.",
                          cursed );

                Dialog::Message( title, msg, Font::BIG, Dialog::OK );
            }
        }
        else {
            Dialog::Message( title, _( "You hear a voice from behind the locked door, \"You don't have enough gold to pay for my services.\"" ), Font::BIG, Dialog::OK );
        }
    }
    else {
        Dialog::Message( title, _( "You hear a voice from high above in the tower, \"Go away! I can't help you!\"" ), Font::BIG, Dialog::OK );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToStables( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    const bool isCavalryPresent = hero.GetArmy().HasMonster( Monster::CAVALRY );
    const bool visited = hero.isObjectTypeVisited( objectType );
    std::string body;

    if ( isCavalryPresent && visited ) {
        body = _(
            "The head groom speaks to you, \"That is a fine looking horse you have. I am afraid we can give you no better, but the horses your cavalry are riding look to be of poor breeding stock. We have many trained war horses which would aid your riders greatly. I insist you take them.\"" );
    }
    if ( isCavalryPresent && !visited ) {
        body = _(
            "As you approach the stables, the head groom appears, leading a fine looking war horse. \"This steed will help speed you in your travels. Alas, he will grow tired in a week. You must also let me give better horses to your mounted soldiers, their horses look shoddy and weak.\"" );
    }
    else if ( !isCavalryPresent && visited ) {
        body = _(
            "The head groom approaches you and speaks, \"You already have a fine horse, and have no inexperienced cavalry which might make use of our trained war horses.\"" );
    }
    else {
        body = _(
            "As you approach the stables, the head groom appears, leading a fine looking war horse. \"This steed will help speed you in your travels. Alas, his endurance will wane with a lot of heavy riding, and you must return for a fresh mount in a week. We also have many fine war horses which could benefit mounted soldiers, but you have none we can help.\"" );
    }

    // check if already visited
    if ( !visited ) {
        hero.SetVisited( dst_index );
        AGG::PlaySound( M82::EXPERNCE );
        hero.IncreaseMovePoints( 400 );
    }

    if ( isCavalryPresent ) {
        ActionToUpgradeArmyObject( hero, objectType, body );
    }
    else {
        Dialog::Message( MP2::StringObject( objectType ), body, Font::BIG, Dialog::OK );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToArena( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    if ( hero.isObjectTypeVisited( objectType ) ) {
        Dialog::Message( MP2::StringObject( objectType ), _( "The Arena guards turn you away." ), Font::BIG, Dialog::OK );
    }
    else {
        hero.SetVisited( dst_index );
        AGG::PlaySound( M82::EXPERNCE );
        hero.IncreasePrimarySkill( Dialog::SelectSkillFromArena() );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToSirens( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    const std::string title( MP2::StringObject( objectType ) );

    if ( hero.isObjectTypeVisited( objectType ) ) {
        Dialog::Message( title, _( "As the sirens sing their eerie song, your small, determined army manages to overcome the urge to dive headlong into the sea." ),
                         Font::BIG, Dialog::OK );
    }
    else {
        u32 exp = hero.GetArmy().ActionToSirens();
        std::string str = _(
            "You have your crew stop up their ears with wax before the sirens' eerie song has any chance of luring them to a watery grave. An eerie wailing song emanates from the sirens perched upon the rocks. Many of your crew fall under its spell, and dive into the water where they drown. You are now wiser for the visit, and gain %{exp} experience." );
        StringReplace( str, "%{exp}", exp );

        hero.SetVisited( dst_index );
        AGG::PlaySound( M82::EXPERNCE );
        Dialog::Message( title, str, Font::BIG, Dialog::OK );
        hero.IncreaseExperience( exp );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToJail( const Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    const Kingdom & kingdom = hero.GetKingdom();
    const std::string title( MP2::StringObject( objectType ) );

    if ( kingdom.AllowRecruitHero( false, 0 ) ) {
        Maps::Tiles & tile = world.GetTiles( dst_index );
        AGG::PlaySound( M82::EXPERNCE );
        Dialog::Message(
            title,
            _( "In a dazzling display of daring, you break into the local jail and free the hero imprisoned there, who, in return, pledges loyalty to your cause." ),
            Font::BIG, Dialog::OK );

        Game::ObjectFadeAnimation::PrepareFadeTask( tile.GetObject(), tile.GetIndex(), -1, true, false );

        tile.RemoveObjectSprite();
        tile.setAsEmpty();

        Game::ObjectFadeAnimation::PerformFadeTask();

        Heroes * prisoner = world.FromJailHeroes( dst_index );

        if ( prisoner ) {
            if ( prisoner->Recruit( hero.GetColor(), Maps::GetPoint( dst_index ) ) )
                prisoner->ResetModes( Heroes::JAIL );
        }
    }
    else {
        std::string str = _( "You already have %{count} heroes, and regretfully must leave the prisoner in this jail to languish in agony for untold days." );
        StringReplace( str, "%{count}", Kingdom::GetMaxHeroes() );
        Dialog::Message( title, str, Font::BIG, Dialog::OK );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToHutMagi( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    Dialog::Message( MP2::StringObject( objectType ),
                     _( "You enter a rickety hut and talk to the magician who lives there. He tells you of places near and far which may aid you in your journeys." ),
                     Font::BIG, Dialog::OK );

    if ( !hero.isObjectTypeVisited( objectType, Visit::GLOBAL ) ) {
        hero.SetVisited( dst_index, Visit::GLOBAL );
        MapsIndexes vec_eyes = Maps::GetObjectPositions( MP2::OBJ_EYEMAGI, true );

        if ( !vec_eyes.empty() ) {
            Interface::Basic & I = Interface::Basic::Get();
            for ( MapsIndexes::const_iterator it = vec_eyes.begin(); it != vec_eyes.end(); ++it ) {
                Maps::ClearFog( *it, Game::GetViewDistance( Game::VIEW_MAGI_EYES ), hero.GetColor() );
                I.GetGameArea().SetCenter( Maps::GetPoint( *it ) );
                I.RedrawFocus();
                I.Redraw();

                fheroes2::Display::instance().render();

                LocalEvent & le = LocalEvent::Get();
                int delay = 0;
                while ( le.HandleEvents() && delay < 7 ) {
                    if ( Game::validateAnimationDelay( Game::MAPS_DELAY ) ) {
                        ++delay;
                    }
                }
            }
            I.GetGameArea().SetCenter( hero.GetCenter() );
        }
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToEyeMagi( const Heroes & hero, const MP2::MapObjectType objectType )
{
    Dialog::Message( MP2::StringObject( objectType ), _( "This eye seems to be intently studying its surroundings." ), Font::BIG, Dialog::OK );

    (void)hero;
    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToSphinx( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    MapSphinx * riddle = dynamic_cast<MapSphinx *>( world.GetMapObject( dst_index ) );
    const std::string title = MP2::StringObject( objectType );

    if ( riddle && riddle->valid ) {
        if (
            Dialog::YES
            == Dialog::Message(
                title,
                _( "\"I have a riddle for you,\" the Sphinx says. \"Answer correctly, and you shall be rewarded. Answer incorrectly, and you shall be eaten. Do you accept the challenge?\"" ),
                Font::BIG, Dialog::YES | Dialog::NO ) ) {
            std::string header( _( "The Sphinx asks you the following riddle:\n \n'%{riddle}'\n \nYour answer?" ) );
            StringReplace( header, "%{riddle}", riddle->message );
            std::string answer;
            Dialog::InputString( header, answer, title );
            if ( riddle->AnswerCorrect( answer ) ) {
                const Funds & res = riddle->resources;
                const Artifact art = riddle->artifact;
                const std::string say = _( "Looking somewhat disappointed, the Sphinx sighs. You've answered my riddle so here's your reward. Now begone." );
                const u32 count = res.GetValidItemsCount();

                if ( count ) {
                    if ( 1 == count && res.gold && art.isValid() )
                        DialogWithArtifactAndGold( title, say, art, res.gold );
                    else {
                        Dialog::ResourceInfo( title, say, res );
                        if ( art.isValid() ) {
                            AGG::PlaySound( M82::TREASURE );
                            Dialog::ArtifactInfo( title, say, art );
                        }
                    }
                }
                else if ( art.isValid() ) {
                    AGG::PlaySound( M82::TREASURE );
                    Dialog::ArtifactInfo( title, say, art );
                }

                if ( art.isValid() )
                    hero.PickupArtifact( art );

                if ( count )
                    hero.GetKingdom().AddFundsResource( res );

                riddle->SetQuiet();
                hero.SetVisited( dst_index, Visit::GLOBAL );
            }
            else {
                Dialog::Message(
                    title,
                    _( "\"You guessed incorrectly,\" the Sphinx says, smiling. The Sphinx swipes at you with a paw, knocking you to the ground. Another blow makes the world go black, and you know no more." ),
                    Font::BIG, Dialog::OK );
                Battle::Result res;
                res.army1 = Battle::RESULT_LOSS;
                BattleLose( hero, res, true );
            }
        }
    }
    else {
        Dialog::Message( title, _( "You come across a giant Sphinx. The Sphinx remains strangely quiet." ), Font::BIG, Dialog::OK );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToBarrier( Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    // A hero cannot stand on a barrier. He must stand in front of the barrier. Something wrong with logic!
    assert( hero.GetIndex() != dst_index );

    Maps::Tiles & tile = world.GetTiles( dst_index );
    const Kingdom & kingdom = hero.GetKingdom();

    const std::string title = MP2::StringObject( objectType );

    if ( kingdom.IsVisitTravelersTent( tile.QuantityColor() ) ) {
        Dialog::Message(
            title,
            _( "A magical barrier stands tall before you, blocking your way. Runes on the arch read,\n\"Speak the key and you may pass.\"\nAs you speak the magic word, the glowing barrier dissolves into nothingness." ),
            Font::BIG, Dialog::OK );

        Game::ObjectFadeAnimation::PrepareFadeTask( tile.GetObject(), tile.GetIndex(), -1, true, false );

        tile.RemoveObjectSprite();
        tile.setAsEmpty();

        AGG::PlaySound( M82::KILLFADE );
        Game::ObjectFadeAnimation::PerformFadeTask();
    }
    else {
        Dialog::Message(
            title,
            _( "A magical barrier stands tall before you, blocking your way. Runes on the arch read,\n\"Speak the key and you may pass.\"\nYou speak, and nothing happens." ),
            Font::BIG, Dialog::OK );
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}

void ActionToTravellersTent( const Heroes & hero, const MP2::MapObjectType objectType, s32 dst_index )
{
    AGG::PlaySound( M82::EXPERNCE );
    Dialog::Message(
        MP2::StringObject( objectType ),
        _( "You enter the tent and see an old woman gazing into a magic gem. She looks up and says,\n\"In my travels, I have learned much in the way of arcane magic. A great oracle taught me his skill. I have the answer you seek.\"" ),
        Font::BIG, Dialog::OK );

    const Maps::Tiles & tile = world.GetTiles( dst_index );
    Kingdom & kingdom = hero.GetKingdom();

    kingdom.SetVisitTravelersTent( tile.QuantityColor() );

    DEBUG_LOG( DBG_GAME, DBG_INFO, hero.GetName() );
}
