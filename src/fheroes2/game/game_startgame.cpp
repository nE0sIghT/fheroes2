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

#include <algorithm>
#include <vector>

#include "agg.h"
#include "agg_image.h"
#include "ai.h"
#include "audio.h"
#include "battle_only.h"
#include "castle.h"
#include "cursor.h"
#include "dialog.h"
#include "game.h"
#include "game_delays.h"
#include "game_interface.h"
#include "game_io.h"
#include "game_over.h"
#include "heroes.h"
#include "icn.h"
#include "kingdom.h"
#include "logging.h"
#include "m82.h"
#include "maps_tiles.h"
#include "mus.h"
#include "route.h"
#include "settings.h"
#include "text.h"
#include "tools.h"
#include "translations.h"
#include "world.h"

namespace
{
    bool SortPlayers( const Player * player1, const Player * player2 )
    {
        return ( player1->isControlHuman() && !player2->isControlHuman() )
               || ( ( player1->isControlHuman() == player2->isControlHuman() ) && ( player1->GetColor() < player2->GetColor() ) );
    }
}

fheroes2::GameMode Game::StartBattleOnly( void )
{
    Battle::Only main;

    if ( main.ChangeSettings() )
        main.StartBattle();

    return fheroes2::GameMode::MAIN_MENU;
}

fheroes2::GameMode Game::StartGame()
{
    AI::Get().Reset();

    const Settings & conf = Settings::Get();

    // setup cursor
    const CursorRestorer cursorRestorer( true, Cursor::POINTER );

    if ( !conf.LoadedGameVersion() )
        GameOver::Result::Get().Reset();

    AGG::ResetMixer( true );

    Interface::Basic::Get().Reset();

    return Interface::Basic::Get().StartGame();
}

void Game::DialogPlayers( int color, std::string str )
{
    const Player * player = Players::Get( color );
    StringReplace( str, "%{color}", ( player ? player->GetName() : Color::String( color ) ) );

    const fheroes2::Sprite & border = fheroes2::AGG::GetICN( ICN::BRCREST, 6 );
    fheroes2::Sprite sign = border;

    switch ( color ) {
    case Color::BLUE:
        fheroes2::Blit( fheroes2::AGG::GetICN( ICN::BRCREST, 0 ), sign, 4, 4 );
        break;
    case Color::GREEN:
        fheroes2::Blit( fheroes2::AGG::GetICN( ICN::BRCREST, 1 ), sign, 4, 4 );
        break;
    case Color::RED:
        fheroes2::Blit( fheroes2::AGG::GetICN( ICN::BRCREST, 2 ), sign, 4, 4 );
        break;
    case Color::YELLOW:
        fheroes2::Blit( fheroes2::AGG::GetICN( ICN::BRCREST, 3 ), sign, 4, 4 );
        break;
    case Color::ORANGE:
        fheroes2::Blit( fheroes2::AGG::GetICN( ICN::BRCREST, 4 ), sign, 4, 4 );
        break;
    case Color::PURPLE:
        fheroes2::Blit( fheroes2::AGG::GetICN( ICN::BRCREST, 5 ), sign, 4, 4 );
        break;
    default:
        break;
    }

    Dialog::SpriteInfo( "", str, sign );
}

/* open castle wrapper */
void Game::OpenCastleDialog( Castle & castle, bool updateFocus /* = true */ )
{
    // setup cursor
    const CursorRestorer cursorRestorer( true, Cursor::POINTER );

    Mixer::Pause();

    const Settings & conf = Settings::Get();
    Kingdom & myKingdom = world.GetKingdom( conf.CurrentColor() );
    const KingdomCastles & myCastles = myKingdom.GetCastles();
    KingdomCastles::const_iterator it = std::find( myCastles.begin(), myCastles.end(), &castle );
    Interface::StatusWindow::ResetTimer();

    const size_t heroCountBefore = myKingdom.GetHeroes().size();

    if ( it != myCastles.end() ) {
        Castle::CastleDialogReturnValue result = Castle::CastleDialogReturnValue::DoNothing;

        while ( result != Castle::CastleDialogReturnValue::Close ) {
            assert( it != myCastles.end() );

            const bool openConstructionWindow
                = ( result == Castle::CastleDialogReturnValue::PreviousCostructionWindow ) || ( result == Castle::CastleDialogReturnValue::NextCostructionWindow );

            result = ( *it )->OpenDialog( false, openConstructionWindow );

            if ( result == Castle::CastleDialogReturnValue::PreviousCastle || result == Castle::CastleDialogReturnValue::PreviousCostructionWindow ) {
                if ( it == myCastles.begin() )
                    it = myCastles.end();
                --it;
            }
            else if ( result == Castle::CastleDialogReturnValue::NextCastle || result == Castle::CastleDialogReturnValue::NextCostructionWindow ) {
                ++it;
                if ( it == myCastles.end() )
                    it = myCastles.begin();
            }
        }
    }
    else if ( castle.isFriends( conf.CurrentColor() ) ) {
        castle.OpenDialog( true, false );
    }

    Interface::Basic & basicInterface = Interface::Basic::Get();

    if ( updateFocus ) {
        if ( heroCountBefore < myKingdom.GetHeroes().size() ) {
            basicInterface.SetFocus( myKingdom.GetHeroes()[heroCountBefore] );
        }
        else if ( it != myCastles.end() ) {
            Heroes * heroInCastle = world.GetTiles( ( *it )->GetIndex() ).GetHeroes();
            if ( heroInCastle == nullptr ) {
                basicInterface.SetFocus( *it );
            }
            else {
                basicInterface.SetFocus( heroInCastle );
            }
        }
        else {
            basicInterface.ResetFocus( GameFocus::HEROES );
        }
    }
    else {
        // If we don't update focus, we still have to restore environment sounds and terrain music theme
        restoreSoundsForCurrentFocus();
    }

    basicInterface.RedrawFocus();
}

/* open heroes wrapper */
void Game::OpenHeroesDialog( Heroes & hero, bool updateFocus, bool windowIsGameWorld, bool disableDismiss /* = false */ )
{
    // setup cursor
    const CursorRestorer cursorRestorer( true, Cursor::POINTER );

    Interface::StatusWindow::ResetTimer();

    const Settings & conf = Settings::Get();
    bool needFade = conf.ExtGameUseFade() && fheroes2::Display::instance().isDefaultSize();

    Interface::Basic & I = Interface::Basic::Get();
    const Interface::GameArea & gameArea = I.GetGameArea();

    const KingdomHeroes & myHeroes = hero.GetKingdom().GetHeroes();
    KingdomHeroes::const_iterator it = std::find( myHeroes.begin(), myHeroes.end(), &hero );

    int result = Dialog::ZERO;

    while ( it != myHeroes.end() && result != Dialog::CANCEL ) {
        result = ( *it )->OpenDialog( false, needFade, disableDismiss );
        if ( needFade )
            needFade = false;

        switch ( result ) {
        case Dialog::PREV:
            if ( it == myHeroes.begin() )
                it = myHeroes.end();
            --it;
            break;

        case Dialog::NEXT:
            ++it;
            if ( it == myHeroes.end() )
                it = myHeroes.begin();
            break;

        case Dialog::DISMISS:
            AGG::PlaySound( M82::KILLFADE );

            ( *it )->GetPath().Hide();
            gameArea.SetRedraw();

            if ( windowIsGameWorld ) {
                ( *it )->FadeOut();
            }

            ( *it )->SetFreeman( 0 );
            it = myHeroes.end();

            updateFocus = true;

            result = Dialog::CANCEL;
            break;

        default:
            break;
        }
    }

    if ( updateFocus ) {
        if ( it != myHeroes.end() ) {
            I.SetFocus( *it );
        }
        else {
            I.ResetFocus( GameFocus::HEROES );
        }
    }

    I.RedrawFocus();
}

void ShowNewWeekDialog( void )
{
    // restore the original music on exit
    const Game::MusicRestorer musicRestorer;

    AGG::PlayMusic( world.BeginMonth() ? MUS::NEW_MONTH : MUS::NEW_WEEK, false );

    const Week & week = world.GetWeekType();

    // head
    std::string message = world.BeginMonth() ? _( "Astrologers proclaim Month of the %{name}." ) : _( "Astrologers proclaim Week of the %{name}." );
    StringReplace( message, "%{name}", week.GetName() );
    message += "\n \n";

    if ( week.GetType() == WeekName::MONSTERS ) {
        const Monster monster( week.GetMonster() );
        const u32 count = world.BeginMonth() ? Castle::GetGrownMonthOf() : Castle::GetGrownWeekOf();

        if ( monster.isValid() && count ) {
            if ( world.BeginMonth() )
                message += 100 == Castle::GetGrownMonthOf() ? _( "After regular growth, the population of %{monster} is doubled!" )
                                                            : _n( "After regular growth, the population of %{monster} increases by %{count} percent!",
                                                                  "After regular growth, the population of %{monster} increases by %{count} percent!", count );
            else
                message += _( "%{monster} population increases by +%{count}." );
            StringReplace( message, "%{monster}", monster.GetMultiName() );
            StringReplace( message, "%{count}", count );
            message += "\n \n";
        }
    }

    if ( week.GetType() == WeekName::PLAGUE )
        message += _( " All populations are halved." );
    else
        message += _( " All dwellings increase population." );

    Dialog::Message( "", message, Font::BIG, Dialog::OK );
}

void ShowEventDayDialog( void )
{
    const Kingdom & myKingdom = world.GetKingdom( Settings::Get().CurrentColor() );
    const EventsDate events = world.GetEventsDate( myKingdom.GetColor() );

    for ( const EventDate & event : events ) {
        if ( event.resource.GetValidItemsCount() ) {
            Dialog::ResourceInfo( event.title, event.message, event.resource );
        }
        else if ( !event.message.empty() ) {
            Dialog::Message( event.title, event.message, Font::BIG, Dialog::OK );
        }
    }
}

void ShowWarningLostTownsDialog()
{
    const Kingdom & myKingdom = world.GetKingdom( Settings::Get().CurrentColor() );
    const uint32_t lostTownDays = myKingdom.GetLostTownDays();

    if ( lostTownDays == 1 ) {
        Game::DialogPlayers( myKingdom.GetColor(), _( "%{color} player, this is your last day to capture a town, or you will be banished from this land." ) );
    }
    else if ( lostTownDays > 0 && lostTownDays <= Game::GetLostTownDays() ) {
        std::string str = _( "%{color} player, you only have %{day} days left to capture a town, or you will be banished from this land." );
        StringReplace( str, "%{day}", lostTownDays );
        Game::DialogPlayers( myKingdom.GetColor(), str );
    }
}

int Interface::Basic::GetCursorFocusCastle( const Castle & from_castle, const Maps::Tiles & tile )
{
    switch ( tile.GetObject() ) {
    case MP2::OBJN_CASTLE:
    case MP2::OBJ_CASTLE: {
        const Castle * to_castle = world.getCastle( tile.GetCenter() );

        if ( nullptr != to_castle )
            return to_castle->GetColor() == from_castle.GetColor() ? Cursor::CASTLE : Cursor::POINTER;
        break;
    }

    case MP2::OBJ_HEROES: {
        const Heroes * heroes = tile.GetHeroes();

        if ( nullptr != heroes )
            return heroes->GetColor() == from_castle.GetColor() ? Cursor::HEROES : Cursor::POINTER;
        break;
    }

    default:
        break;
    }

    return Cursor::POINTER;
}

int Interface::Basic::GetCursorFocusShipmaster( const Heroes & from_hero, const Maps::Tiles & tile )
{
    const bool water = tile.isWater();

    switch ( tile.GetObject() ) {
    case MP2::OBJ_MONSTER:
        return water ? Cursor::DistanceThemes( Cursor::CURSOR_HERO_FIGHT, from_hero.GetRangeRouteDays( tile.GetIndex() ) ) : Cursor::POINTER;

    case MP2::OBJ_BOAT:
        return Cursor::POINTER;

    case MP2::OBJN_CASTLE:
    case MP2::OBJ_CASTLE: {
        const Castle * castle = world.getCastle( tile.GetCenter() );

        if ( castle )
            return from_hero.GetColor() == castle->GetColor() ? Cursor::CASTLE : Cursor::POINTER;
        break;
    }

    case MP2::OBJ_HEROES: {
        const Heroes * to_hero = tile.GetHeroes();

        if ( to_hero ) {
            if ( !to_hero->isShipMaster() )
                return from_hero.GetColor() == to_hero->GetColor() ? Cursor::HEROES : Cursor::POINTER;
            else if ( to_hero->GetCenter() == from_hero.GetCenter() )
                return Cursor::HEROES;
            else if ( from_hero.GetColor() == to_hero->GetColor() )
                return Cursor::DistanceThemes( Cursor::CURSOR_HERO_MEET, from_hero.GetRangeRouteDays( tile.GetIndex() ) );
            else if ( from_hero.isFriends( to_hero->GetColor() ) )
                return Cursor::POINTER;
            else
                return Cursor::DistanceThemes( Cursor::CURSOR_HERO_FIGHT, from_hero.GetRangeRouteDays( tile.GetIndex() ) );
        }
        break;
    }

    case MP2::OBJ_COAST:
        return Cursor::DistanceThemes( Cursor::CURSOR_HERO_ANCHOR, from_hero.GetRangeRouteDays( tile.GetIndex() ) );

    default:
        if ( water ) {
            if ( MP2::isWaterActionObject( tile.GetObject() ) )
                return Cursor::DistanceThemes( Cursor::CURSOR_HERO_BOAT_ACTION, from_hero.GetRangeRouteDays( tile.GetIndex() ) );
            else if ( tile.isPassable( Direction::CENTER, true, false, from_hero.GetColor() ) )
                return Cursor::DistanceThemes( Cursor::CURSOR_HERO_BOAT, from_hero.GetRangeRouteDays( tile.GetIndex() ) );
        }
        break;
    }

    return Cursor::POINTER;
}

int Interface::Basic::GetCursorFocusHeroes( const Heroes & from_hero, const Maps::Tiles & tile )
{
    if ( from_hero.Modes( Heroes::ENABLEMOVE ) )
        return Cursor::Get().Themes();
    else if ( from_hero.isShipMaster() )
        return GetCursorFocusShipmaster( from_hero, tile );

    switch ( tile.GetObject() ) {
    case MP2::OBJ_MONSTER:
        if ( from_hero.Modes( Heroes::GUARDIAN ) )
            return Cursor::POINTER;
        else
            return Cursor::DistanceThemes( Cursor::CURSOR_HERO_FIGHT, from_hero.GetRangeRouteDays( tile.GetIndex() ) );

    case MP2::OBJN_CASTLE:
    case MP2::OBJ_CASTLE: {
        const Castle * castle = world.getCastle( tile.GetCenter() );

        if ( nullptr != castle ) {
            if ( tile.GetObject() == MP2::OBJN_CASTLE ) {
                if ( tile.GetPassable() == 0 ) {
                    return ( from_hero.GetColor() == castle->GetColor() ) ? Cursor::CASTLE : Cursor::POINTER;
                }
                else {
                    const bool protection = Maps::TileIsUnderProtection( tile.GetIndex() );

                    return Cursor::DistanceThemes( ( protection ? Cursor::CURSOR_HERO_FIGHT : Cursor::CURSOR_HERO_MOVE ),
                                                   from_hero.GetRangeRouteDays( tile.GetIndex() ) );
                }
            }
            else if ( from_hero.Modes( Heroes::GUARDIAN ) || from_hero.GetIndex() == castle->GetIndex() ) {
                return from_hero.GetColor() == castle->GetColor() ? Cursor::CASTLE : Cursor::POINTER;
            }
            else if ( from_hero.GetColor() == castle->GetColor() ) {
                return Cursor::DistanceThemes( Cursor::CURSOR_HERO_ACTION, from_hero.GetRangeRouteDays( castle->GetIndex() ) );
            }
            else if ( from_hero.isFriends( castle->GetColor() ) ) {
                return Cursor::POINTER;
            }
            else if ( castle->GetActualArmy().isValid() ) {
                return Cursor::DistanceThemes( Cursor::CURSOR_HERO_FIGHT, from_hero.GetRangeRouteDays( castle->GetIndex() ) );
            }
            else {
                return Cursor::DistanceThemes( Cursor::CURSOR_HERO_ACTION, from_hero.GetRangeRouteDays( castle->GetIndex() ) );
            }
        }
        break;
    }

    case MP2::OBJ_HEROES: {
        const Heroes * to_hero = tile.GetHeroes();

        if ( nullptr != to_hero ) {
            if ( from_hero.Modes( Heroes::GUARDIAN ) )
                return from_hero.GetColor() == to_hero->GetColor() ? Cursor::HEROES : Cursor::POINTER;
            else if ( to_hero->GetCenter() == from_hero.GetCenter() )
                return Cursor::HEROES;
            else if ( from_hero.GetColor() == to_hero->GetColor() ) {
                int newcur = Cursor::DistanceThemes( Cursor::CURSOR_HERO_MEET, from_hero.GetRangeRouteDays( tile.GetIndex() ) );
                return newcur != Cursor::POINTER ? newcur : Cursor::HEROES;
            }
            else if ( from_hero.isFriends( to_hero->GetColor() ) ) {
                return Cursor::POINTER;
            }
            else
                return Cursor::DistanceThemes( Cursor::CURSOR_HERO_FIGHT, from_hero.GetRangeRouteDays( tile.GetIndex() ) );
        }
        break;
    }

    case MP2::OBJ_BOAT:
        return from_hero.Modes( Heroes::GUARDIAN ) ? Cursor::POINTER : Cursor::DistanceThemes( Cursor::CURSOR_HERO_BOAT, from_hero.GetRangeRouteDays( tile.GetIndex() ) );
    case MP2::OBJ_BARRIER:
        return Cursor::DistanceThemes( Cursor::CURSOR_HERO_ACTION, from_hero.GetRangeRouteDays( tile.GetIndex() ) );
    default:
        if ( from_hero.Modes( Heroes::GUARDIAN ) )
            return Cursor::POINTER;
        else if ( MP2::isActionObject( tile.GetObject() ) ) {
            bool protection = false;
            if ( !MP2::isPickupObject( tile.GetObject() ) && !MP2::isAbandonedMine( tile.GetObject() ) ) {
                protection = ( Maps::TileIsUnderProtection( tile.GetIndex() ) || ( !from_hero.isFriends( tile.QuantityColor() ) && tile.CaptureObjectIsProtection() ) );
            }
            else {
                protection = Maps::TileIsUnderProtection( tile.GetIndex() );
            }

            return Cursor::DistanceThemes( ( protection ? Cursor::CURSOR_HERO_FIGHT : Cursor::CURSOR_HERO_ACTION ), from_hero.GetRangeRouteDays( tile.GetIndex() ) );
        }
        else if ( tile.isPassable( Direction::CENTER, from_hero.isShipMaster(), false, from_hero.GetColor() ) ) {
            bool protection = Maps::TileIsUnderProtection( tile.GetIndex() );

            return Cursor::DistanceThemes( ( protection ? Cursor::CURSOR_HERO_FIGHT : Cursor::CURSOR_HERO_MOVE ), from_hero.GetRangeRouteDays( tile.GetIndex() ) );
        }
        break;
    }

    return Cursor::POINTER;
}

int Interface::Basic::GetCursorTileIndex( s32 dst_index )
{
    if ( dst_index < 0 || dst_index >= world.w() * world.h() )
        return Cursor::POINTER;

    const Maps::Tiles & tile = world.GetTiles( dst_index );
    if ( tile.isFog( Settings::Get().CurrentColor() ) )
        return Cursor::POINTER;

    switch ( GetFocusType() ) {
    case GameFocus::HEROES:
        return GetCursorFocusHeroes( *GetFocusHeroes(), tile );

    case GameFocus::CASTLE:
        return GetCursorFocusCastle( *GetFocusCastle(), tile );

    default:
        break;
    }

    return Cursor::POINTER;
}

fheroes2::GameMode Interface::Basic::StartGame()
{
    Settings & conf = Settings::Get();
    fheroes2::Display & display = fheroes2::Display::instance();

    // draw interface
    gameArea.generate( { display.width(), display.height() }, conf.ExtGameHideInterface() );

    radar.Build();
    radar.SetHide( true );

    iconsPanel.ResetIcons();
    iconsPanel.HideIcons();

    statusWindow.Reset();

    if ( conf.ExtGameHideInterface() )
        SetHideInterface( true );

    Redraw( REDRAW_RADAR | REDRAW_ICONS | REDRAW_BUTTONS | REDRAW_STATUS | REDRAW_BORDER );

    bool loadedFromSave = conf.LoadedGameVersion();
    bool skipTurns = loadedFromSave;

    GameOver::Result & gameResult = GameOver::Result::Get();
    fheroes2::GameMode res = fheroes2::GameMode::END_TURN;

    std::vector<Player *> sortedPlayers = conf.GetPlayers();
    std::sort( sortedPlayers.begin(), sortedPlayers.end(), SortPlayers );

    while ( res == fheroes2::GameMode::END_TURN ) {
        if ( !loadedFromSave ) {
            world.NewDay();
        }

        // check if the game is over at the beginning of a new day
        res = gameResult.LocalCheckGameOver();

        if ( res != fheroes2::GameMode::CANCEL ) {
            break;
        }

        res = fheroes2::GameMode::END_TURN;

        for ( const Player * player : sortedPlayers ) {
            assert( player != nullptr );

            Kingdom & kingdom = world.GetKingdom( player->GetColor() );

            if ( skipTurns && !player->isColor( conf.CurrentColor() ) ) {
                continue;
            }

            // player with conf.CurrentColor() was found, there is no need for further skips
            skipTurns = false;

            if ( kingdom.isPlay() ) {
                DEBUG_LOG( DBG_GAME, DBG_INFO,
                           world.DateString() << ", color: " << Color::String( player->GetColor() ) << ", resource: " << kingdom.GetFunds().String() );

                radar.SetHide( true );
                radar.SetRedraw();

                switch ( kingdom.GetControl() ) {
                case CONTROL_HUMAN:
                    if ( conf.IsGameType( Game::TYPE_HOTSEAT ) ) {
                        // we need to hide the world map in hot seat mode
                        conf.SetCurrentColor( -1 );

                        iconsPanel.HideIcons();
                        statusWindow.Reset();

                        SetRedraw( REDRAW_GAMEAREA | REDRAW_STATUS | REDRAW_ICONS );
                        Redraw();
                        display.render();

                        Game::DialogPlayers( player->GetColor(), _( "%{color} player's turn." ) );
                    }

                    conf.SetCurrentColor( player->GetColor() );

                    world.ClearFog( player->GetColor() );

                    kingdom.ActionBeforeTurn();

                    iconsPanel.ShowIcons();
                    iconsPanel.SetRedraw();

                    res = HumanTurn( loadedFromSave );
                    break;

                // CONTROL_AI turn
                default:
                    // TODO: remove this temporary assertion
                    assert( res == fheroes2::GameMode::END_TURN );

                    Cursor::Get().SetThemes( Cursor::WAIT );

                    conf.SetCurrentColor( player->GetColor() );

                    statusWindow.Reset();
                    statusWindow.SetState( StatusType::STATUS_AITURN );

                    Redraw();
                    display.render();

                    world.ClearFog( player->GetColor() );

                    kingdom.ActionBeforeTurn();

                    AI::Get().KingdomTurn( kingdom );
                    break;
                }

                if ( res != fheroes2::GameMode::END_TURN ) {
                    break;
                }

                // check if the game is over after each player's turn
                res = gameResult.LocalCheckGameOver();

                if ( res != fheroes2::GameMode::CANCEL ) {
                    break;
                }

                res = fheroes2::GameMode::END_TURN;
            }

            // reset this after potential HumanTurn() call, but regardless of whether current kingdom
            // is vanquished - next alive kingdom should start a new day from scratch
            loadedFromSave = false;
        }

        // we went through all the players, but the current player from the save file is still not found,
        // something is clearly wrong here
        if ( skipTurns ) {
            DEBUG_LOG( DBG_GAME, DBG_WARN,
                       "the current player from the save file was not found"
                           << ", player color: " << Color::String( conf.CurrentColor() ) );

            res = fheroes2::GameMode::MAIN_MENU;
        }

        // don't carry the current color from the last player to the next turn
        conf.SetCurrentColor( -1 );
    }

    // if we are here, the res value should never be fheroes2::GameMode::END_TURN
    assert( res != fheroes2::GameMode::END_TURN );

    if ( conf.ExtGameUseFade() )
        fheroes2::FadeDisplay();

    return res;
}

fheroes2::GameMode Interface::Basic::HumanTurn( bool isload )
{
    fheroes2::GameMode res = fheroes2::GameMode::CANCEL;

    const Settings & conf = Settings::Get();

    Kingdom & myKingdom = world.GetKingdom( conf.CurrentColor() );
    const KingdomCastles & myCastles = myKingdom.GetCastles();

    // current music will be set along with the focus, reset environment sounds
    // and terrain music theme from the previous turn
    Game::SetCurrentMusic( MUS::UNKNOWN );
    AGG::ResetMixer();

    // set focus
    if ( conf.ExtGameRememberLastFocus() ) {
        if ( GetFocusHeroes() != nullptr )
            ResetFocus( GameFocus::HEROES );
        else if ( GetFocusCastle() != nullptr )
            ResetFocus( GameFocus::CASTLE );
        else
            ResetFocus( GameFocus::FIRSTHERO );
    }
    else {
        ResetFocus( GameFocus::FIRSTHERO );
    }

    radar.SetHide( false );
    statusWindow.Reset();
    gameArea.SetUpdateCursor();
    Redraw( REDRAW_GAMEAREA | REDRAW_RADAR | REDRAW_ICONS | REDRAW_BUTTONS | REDRAW_STATUS | REDRAW_BORDER );

    Game::EnvironmentSoundMixer();

    fheroes2::Display & display = fheroes2::Display::instance();

    display.render();

    if ( !isload ) {
        // new week dialog
        if ( 1 < world.CountWeek() && world.BeginWeek() ) {
            ShowNewWeekDialog();
        }

        // show event day
        ShowEventDayDialog();

        // autosave
        if ( conf.ExtGameAutosaveOn() && conf.ExtGameAutosaveBeginOfDay() )
            Game::AutoSave();
    }

    GameOver::Result & gameResult = GameOver::Result::Get();

    // check if the game is over at the beginning of each human-controlled player's turn
    res = gameResult.LocalCheckGameOver();

    // warn that all the towns are lost
    if ( res == fheroes2::GameMode::CANCEL && myCastles.empty() ) {
        ShowWarningLostTownsDialog();
    }

    int fastScrollRepeatCount = 0;
    const int fastScrollStartThreshold = 2;

    bool isMovingHero = false;
    bool stopHero = false;

    int heroAnimationFrameCount = 0;
    fheroes2::Point heroAnimationOffset;
    int heroAnimationSpriteId = 0;

    bool isCursorOverButtons = false;

    const std::vector<Game::DelayType> delayTypes = { Game::CURRENT_HERO_DELAY, Game::MAPS_DELAY };

    LocalEvent & le = LocalEvent::Get();
    Cursor & cursor = Cursor::Get();

    // startgame loop
    while ( fheroes2::GameMode::CANCEL == res ) {
        if ( !le.HandleEvents( Game::isDelayNeeded( delayTypes ), true ) ) {
            if ( EventExit() == fheroes2::GameMode::QUIT_GAME ) {
                res = fheroes2::GameMode::QUIT_GAME;
                break;
            }
            else {
                continue;
            }
        }

        // hot keys
        if ( le.KeyPress() ) {
            // stop moving hero first if needed
            if ( isMovingHero )
                stopHero = true;
            // exit dialog
            else if ( HotKeyPressEvent( Game::EVENT_DEFAULT_EXIT ) )
                res = EventExit();
            // end turn
            else if ( HotKeyPressEvent( Game::EVENT_ENDTURN ) )
                res = EventEndTurn();
            // next hero
            else if ( HotKeyPressEvent( Game::EVENT_NEXTHERO ) )
                EventNextHero();
            // next town
            else if ( HotKeyPressEvent( Game::EVENT_NEXTTOWN ) )
                EventNextTown();
            // new game
            else if ( HotKeyPressEvent( Game::EVENT_BUTTON_NEWGAME ) )
                res = EventNewGame();
            // save game
            else if ( HotKeyPressEvent( Game::EVENT_SAVEGAME ) )
                EventSaveGame();
            // load game
            else if ( HotKeyPressEvent( Game::EVENT_LOADGAME ) ) {
                res = EventLoadGame();
            }
            // file options
            else if ( HotKeyPressEvent( Game::EVENT_FILEOPTIONS ) )
                res = EventFileDialog();
            // system options
            else if ( HotKeyPressEvent( Game::EVENT_SYSTEMOPTIONS ) )
                EventSystemDialog();
            // puzzle map
            else if ( HotKeyPressEvent( Game::EVENT_PUZZLEMAPS ) )
                EventPuzzleMaps();
            // info game
            else if ( HotKeyPressEvent( Game::EVENT_INFOGAME ) )
                res = EventGameInfo();
            // cast spell
            else if ( HotKeyPressEvent( Game::EVENT_CASTSPELL ) )
                EventCastSpell();
            // kingdom overview
            else if ( HotKeyPressEvent( Game::EVENT_KINGDOM_INFO ) )
                EventKingdomInfo();
            // view world
            else if ( HotKeyPressEvent( Game::EVENT_VIEW_WORLD ) )
                EventViewWorld();
            // show/hide control panel
            else if ( HotKeyPressEvent( Game::EVENT_CTRLPANEL ) )
                EventSwitchShowControlPanel();
            // hide/show radar
            else if ( HotKeyPressEvent( Game::EVENT_SHOWRADAR ) )
                EventSwitchShowRadar();
            // hide/show buttons
            else if ( HotKeyPressEvent( Game::EVENT_SHOWBUTTONS ) )
                EventSwitchShowButtons();
            // hide/show status window
            else if ( HotKeyPressEvent( Game::EVENT_SHOWSTATUS ) )
                EventSwitchShowStatus();
            // hide/show hero/town icons
            else if ( HotKeyPressEvent( Game::EVENT_SHOWICONS ) )
                EventSwitchShowIcons();
            // hero movement
            else if ( HotKeyPressEvent( Game::EVENT_CONTINUE ) )
                EventContinueMovement();
            // dig artifact
            else if ( HotKeyPressEvent( Game::EVENT_DIGARTIFACT ) )
                res = EventDigArtifact();
            // sleep hero
            else if ( HotKeyPressEvent( Game::EVENT_SLEEPHERO ) )
                EventSwitchHeroSleeping();
            // move hero
            else if ( HotKeyPressEvent( Game::EVENT_MOVELEFT ) )
                EventKeyArrowPress( Direction::LEFT );
            else if ( HotKeyPressEvent( Game::EVENT_MOVERIGHT ) )
                EventKeyArrowPress( Direction::RIGHT );
            else if ( HotKeyPressEvent( Game::EVENT_MOVETOP ) )
                EventKeyArrowPress( Direction::TOP );
            else if ( HotKeyPressEvent( Game::EVENT_MOVEBOTTOM ) )
                EventKeyArrowPress( Direction::BOTTOM );
            else if ( HotKeyPressEvent( Game::EVENT_MOVETOPLEFT ) )
                EventKeyArrowPress( Direction::TOP_LEFT );
            else if ( HotKeyPressEvent( Game::EVENT_MOVETOPRIGHT ) )
                EventKeyArrowPress( Direction::TOP_RIGHT );
            else if ( HotKeyPressEvent( Game::EVENT_MOVEBOTTOMLEFT ) )
                EventKeyArrowPress( Direction::BOTTOM_LEFT );
            else if ( HotKeyPressEvent( Game::EVENT_MOVEBOTTOMRIGHT ) )
                EventKeyArrowPress( Direction::BOTTOM_RIGHT );
            // scroll maps
            else if ( HotKeyPressEvent( Game::EVENT_SCROLLLEFT ) )
                gameArea.SetScroll( SCROLL_LEFT );
            else if ( HotKeyPressEvent( Game::EVENT_SCROLLRIGHT ) )
                gameArea.SetScroll( SCROLL_RIGHT );
            else if ( HotKeyPressEvent( Game::EVENT_SCROLLUP ) )
                gameArea.SetScroll( SCROLL_TOP );
            else if ( HotKeyPressEvent( Game::EVENT_SCROLLDOWN ) )
                gameArea.SetScroll( SCROLL_BOTTOM );
            // default action
            else if ( HotKeyPressEvent( Game::EVENT_DEFAULTACTION ) )
                res = EventDefaultAction( res );
            // open focus
            else if ( HotKeyPressEvent( Game::EVENT_OPENFOCUS ) )
                EventOpenFocus();
        }

        if ( res != fheroes2::GameMode::CANCEL ) {
            break;
        }

        if ( fheroes2::cursor().isFocusActive() ) {
            int scrollPosition = SCROLL_NONE;

            if ( le.MouseCursor( GetScrollLeft() ) )
                scrollPosition |= SCROLL_LEFT;
            else if ( le.MouseCursor( GetScrollRight() ) )
                scrollPosition |= SCROLL_RIGHT;
            if ( le.MouseCursor( GetScrollTop() ) )
                scrollPosition |= SCROLL_TOP;
            else if ( le.MouseCursor( GetScrollBottom() ) )
                scrollPosition |= SCROLL_BOTTOM;

            if ( scrollPosition != SCROLL_NONE ) {
                if ( Game::validateAnimationDelay( Game::SCROLL_START_DELAY ) ) {
                    if ( fastScrollRepeatCount < fastScrollStartThreshold ) {
                        ++fastScrollRepeatCount;
                    }
                }

                if ( fastScrollRepeatCount >= fastScrollStartThreshold ) {
                    gameArea.SetScroll( scrollPosition );
                }
            }
            else {
                fastScrollRepeatCount = 0;
            }
        }
        else {
            fastScrollRepeatCount = 0;
        }

        const fheroes2::Rect displayArea( 0, 0, display.width(), display.height() );
        const bool isHiddenInterface = conf.ExtGameHideInterface();
        const bool prevIsCursorOverButtons = isCursorOverButtons;
        isCursorOverButtons = false;

        if ( isMovingHero ) {
            // hero is moving, set the appropriate cursor
            if ( cursor.Themes() != Cursor::WAIT ) {
                cursor.SetThemes( Cursor::WAIT );
            }

            // stop moving hero if needed
            if ( le.MouseClickLeft( displayArea ) || le.MousePressRight( displayArea ) ) {
                stopHero = true;
            }
        }
        // cursor over radar
        else if ( ( !isHiddenInterface || conf.ShowRadar() ) && le.MouseCursor( radar.GetRect() ) ) {
            if ( Cursor::POINTER != cursor.Themes() )
                cursor.SetThemes( Cursor::POINTER );
            radar.QueueEventProcessing();
        }
        // cursor over icons panel
        else if ( ( !isHiddenInterface || conf.ShowIcons() ) && le.MouseCursor( iconsPanel.GetRect() ) ) {
            if ( Cursor::POINTER != cursor.Themes() )
                cursor.SetThemes( Cursor::POINTER );
            iconsPanel.QueueEventProcessing();
        }
        // cursor over buttons area
        else if ( ( !isHiddenInterface || conf.ShowButtons() ) && le.MouseCursor( buttonsArea.GetRect() ) ) {
            if ( Cursor::POINTER != cursor.Themes() )
                cursor.SetThemes( Cursor::POINTER );
            res = buttonsArea.QueueEventProcessing();
            isCursorOverButtons = true;
        }
        // cursor over status area
        else if ( ( !isHiddenInterface || conf.ShowStatus() ) && le.MouseCursor( statusWindow.GetRect() ) ) {
            if ( Cursor::POINTER != cursor.Themes() )
                cursor.SetThemes( Cursor::POINTER );
            statusWindow.QueueEventProcessing();
        }
        // cursor over control panel
        else if ( isHiddenInterface && conf.ShowControlPanel() && le.MouseCursor( controlPanel.GetArea() ) ) {
            if ( Cursor::POINTER != cursor.Themes() )
                cursor.SetThemes( Cursor::POINTER );
            res = controlPanel.QueueEventProcessing();
        }
        // cursor over game area
        else if ( le.MouseCursor( gameArea.GetROI() ) && !gameArea.NeedScroll() ) {
            gameArea.QueueEventProcessing();
        }
        else if ( !gameArea.NeedScroll() ) { // empty interface area so we set cursor to a normal pointer
            if ( Cursor::POINTER != cursor.Themes() )
                cursor.SetThemes( Cursor::POINTER );
            gameArea.ResetCursorPosition();
        }

        if ( prevIsCursorOverButtons && !isCursorOverButtons ) {
            buttonsArea.ResetButtons();
        }

        if ( res != fheroes2::GameMode::CANCEL ) {
            break;
        }

        // heroes move animation
        if ( Game::validateAnimationDelay( Game::CURRENT_HERO_DELAY ) ) {
            Heroes * hero = GetFocusHeroes();

            if ( hero ) {
                bool resetHeroSprite = false;
                if ( heroAnimationFrameCount > 0 ) {
                    gameArea.ShiftCenter( fheroes2::Point( heroAnimationOffset.x * Game::HumanHeroAnimSkip(), heroAnimationOffset.y * Game::HumanHeroAnimSkip() ) );
                    gameArea.SetRedraw();
                    heroAnimationFrameCount -= Game::HumanHeroAnimSkip();
                    if ( ( heroAnimationFrameCount & 0x3 ) == 0 ) { // % 4
                        hero->SetSpriteIndex( heroAnimationSpriteId );

                        if ( heroAnimationFrameCount == 0 )
                            resetHeroSprite = true;
                        else
                            ++heroAnimationSpriteId;
                    }
                    const int offsetStep = ( ( 4 - ( heroAnimationFrameCount & 0x3 ) ) & 0x3 ); // % 4
                    hero->SetOffset( fheroes2::Point( heroAnimationOffset.x * offsetStep, heroAnimationOffset.y * offsetStep ) );
                }

                if ( heroAnimationFrameCount == 0 ) {
                    if ( resetHeroSprite ) {
                        hero->SetSpriteIndex( heroAnimationSpriteId - 1 );
                    }
                    if ( hero->isMoveEnabled() ) {
                        if ( hero->Move( 10 == conf.HeroesMoveSpeed() ) ) {
                            gameArea.SetCenter( hero->GetCenter() );
                            ResetFocus( GameFocus::HEROES );
                            RedrawFocus();

                            if ( stopHero ) {
                                stopHero = false;

                                hero->SetMove( false );
                            }
                        }
                        else {
                            fheroes2::Point movement( hero->MovementDirection() );
                            if ( movement != fheroes2::Point() ) { // don't waste resources for no movement
                                heroAnimationOffset = movement;
                                gameArea.ShiftCenter( movement );
                                ResetFocus( GameFocus::HEROES );
                                heroAnimationFrameCount = 32 - Game::HumanHeroAnimSkip();
                                heroAnimationSpriteId = hero->GetSpriteIndex();
                                if ( Game::HumanHeroAnimSkip() < 4 ) {
                                    hero->SetSpriteIndex( heroAnimationSpriteId - 1 );
                                    hero->SetOffset(
                                        fheroes2::Point( heroAnimationOffset.x * Game::HumanHeroAnimSkip(), heroAnimationOffset.y * Game::HumanHeroAnimSkip() ) );
                                }
                                else {
                                    ++heroAnimationSpriteId;
                                }
                            }

                            gameArea.SetRedraw();
                        }

                        isMovingHero = true;

                        if ( hero->isAction() ) {
                            // check if the game is over after the hero's action
                            res = gameResult.LocalCheckGameOver();

                            hero->ResetAction();
                        }
                    }
                    else {
                        isMovingHero = false;
                        stopHero = false;

                        hero->SetMove( false );

                        gameArea.SetUpdateCursor();
                    }
                }
            }
            else {
                isMovingHero = false;
                stopHero = false;
            }
        }

        // fast scroll
        if ( gameArea.NeedScroll() && !isMovingHero ) {
            if ( Game::validateAnimationDelay( Game::SCROLL_DELAY ) ) {
                if ( le.MouseCursor( GetScrollLeft() ) || le.MouseCursor( GetScrollRight() ) || le.MouseCursor( GetScrollTop() )
                     || le.MouseCursor( GetScrollBottom() ) ) {
                    cursor.SetThemes( gameArea.GetScrollCursor() );
                }

                gameArea.Scroll();

                gameArea.SetRedraw();
                radar.SetRedraw();
            }
        }

        // slow maps objects animation
        if ( Game::validateAnimationDelay( Game::MAPS_DELAY ) ) {
            u32 & frame = Game::MapsAnimationFrame();
            ++frame;
            gameArea.SetRedraw();
        }

        // check that the kingdom is not vanquished yet (has at least one hero or castle)
        if ( res == fheroes2::GameMode::CANCEL && !myKingdom.isPlay() ) {
            res = fheroes2::GameMode::END_TURN;
        }

        if ( NeedRedraw() ) {
            Redraw();
            display.render();
        }
    }

    if ( fheroes2::GameMode::END_TURN == res ) {
        // these warnings should be shown at the end of the turn
        if ( myKingdom.isPlay() && myCastles.empty() ) {
            const uint32_t lostTownDays = myKingdom.GetLostTownDays();

            if ( lostTownDays > Game::GetLostTownDays() ) {
                Game::DialogPlayers( conf.CurrentColor(),
                                     _( "%{color} player, you have lost your last town. If you do not conquer another town in next week, you will be eliminated." ) );
            }
            else if ( lostTownDays == 1 ) {
                Game::DialogPlayers( conf.CurrentColor(), _( "%{color} player, your heroes abandon you, and you are banished from this land." ) );
            }
        }

        if ( GetFocusHeroes() ) {
            GetFocusHeroes()->ShowPath( false );
            RedrawFocus();
        }

        if ( conf.ExtGameAutosaveOn() && !conf.ExtGameAutosaveBeginOfDay() )
            Game::AutoSave();
    }

    // reset environment sounds and terrain music theme at the end of the human turn
    Game::SetCurrentMusic( MUS::UNKNOWN );
    AGG::ResetMixer();

    return res;
}

void Interface::Basic::MouseCursorAreaClickLeft( const int32_t index_maps )
{
    Heroes * from_hero = GetFocusHeroes();
    const Maps::Tiles & tile = world.GetTiles( index_maps );

    switch ( Cursor::WithoutDistanceThemes( Cursor::Get().Themes() ) ) {
    case Cursor::HEROES: {
        Heroes * to_hero = tile.GetHeroes();
        // focus change/open hero
        if ( nullptr != to_hero ) {
            if ( !from_hero || from_hero != to_hero ) {
                SetFocus( to_hero );
                RedrawFocus();
            }
            else {
                Game::OpenHeroesDialog( *to_hero, true, true );
                Cursor::Get().SetThemes( Cursor::HEROES );
            }
        }
        break;
    }

    case Cursor::CASTLE: {
        // correct index for castle
        const MP2::MapObjectType objectType = tile.GetObject();
        if ( MP2::OBJN_CASTLE != objectType && MP2::OBJ_CASTLE != objectType )
            break;

        Castle * to_castle = world.getCastle( tile.GetCenter() );
        if ( to_castle == nullptr )
            break;

        const Castle * from_castle = GetFocusCastle();
        if ( !from_castle || from_castle != to_castle ) {
            SetFocus( to_castle );
            RedrawFocus();
        }
        else {
            Game::OpenCastleDialog( *to_castle );
            Cursor::Get().SetThemes( Cursor::CASTLE );
        }
        break;
    }
    case Cursor::CURSOR_HERO_FIGHT:
    case Cursor::CURSOR_HERO_MOVE:
    case Cursor::CURSOR_HERO_BOAT:
    case Cursor::CURSOR_HERO_ANCHOR:
    case Cursor::CURSOR_HERO_MEET:
    case Cursor::CURSOR_HERO_ACTION:
    case Cursor::CURSOR_HERO_BOAT_ACTION:
        if ( from_hero == nullptr )
            break;

        if ( from_hero->isMoveEnabled() )
            from_hero->SetMove( false );
        else
            ShowPathOrStartMoveHero( from_hero, index_maps );
        break;

    default:
        if ( from_hero )
            from_hero->SetMove( false );
        break;
    }
}

void Interface::Basic::MouseCursorAreaPressRight( s32 index_maps ) const
{
    Heroes * hero = GetFocusHeroes();

    // stop hero
    if ( hero && hero->isMoveEnabled() ) {
        hero->SetMove( false );
        Cursor::Get().SetThemes( GetCursorTileIndex( index_maps ) );
    }
    else {
        const Settings & conf = Settings::Get();
        const Maps::Tiles & tile = world.GetTiles( index_maps );

        DEBUG_LOG( DBG_DEVEL, DBG_INFO, std::endl << tile.String() );

        if ( !IS_DEVEL() && tile.isFog( conf.CurrentColor() ) )
            Dialog::QuickInfo( tile );
        else
            switch ( tile.GetObject() ) {
            case MP2::OBJN_CASTLE:
            case MP2::OBJ_CASTLE: {
                const Castle * castle = world.getCastle( tile.GetCenter() );
                if ( castle )
                    Dialog::QuickInfo( *castle, fheroes2::Rect() );
                else
                    Dialog::QuickInfo( tile );
                break;
            }

            case MP2::OBJ_HEROES: {
                const Heroes * heroes = tile.GetHeroes();
                if ( heroes )
                    Dialog::QuickInfo( *heroes, fheroes2::Rect() );
                break;
            }

            default:
                Dialog::QuickInfo( tile );
                break;
            }
    }
}
