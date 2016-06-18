#include "FigureAction_private.h"

#include "Formation.h"
#include "Routing.h"
#include "Sound.h"
#include "Walker.h"

#include "Data/CityInfo.h"
#include "Data/Event.h"
#include "Data/Formation.h"
#include "Data/Grid.h"

static void enemyInitial(int walkerId, struct Data_Walker *w, struct Data_Formation *f)
{
	Figure_updatePositionInTileList(walkerId);
	w->graphicOffset = 0;
	FigureRoute_remove(walkerId);
	w->waitTicks--;
	if (w->waitTicks <= 0) {
		if (w->isGhost && w->indexInFormation == 0) {
			if (f->layout == FormationLayout_Enemy8) {
				Sound_Speech_playFile("wavs/drums.wav");
			} else if (f->layout == FormationLayout_Enemy12) {
				Sound_Speech_playFile("wavs/horn2.wav");
			} else {
				Sound_Speech_playFile("wavs/horn1.wav");
			}
		}
		w->isGhost = 0;
		if (f->recentFight) {
			w->actionState = FigureActionState_154_EnemyFighting;
		} else {
			w->destinationX = f->destinationX + w->formationPositionX;
			w->destinationY = f->destinationY + w->formationPositionY;
			if (Routing_getGeneralDirection(w->x, w->y, w->destinationX, w->destinationY) < 8) {
				w->actionState = FigureActionState_153_EnemyMarching;
			}
		}
	}
	if (w->type == Figure_Enemy43_Spear || w->type == Figure_Enemy46_Camel ||
		w->type == Figure_Enemy51_Spear || w->type == Figure_Enemy52_MountedArcher) {
		// missile throwers
		w->waitTicksMissile++;
		int xTile, yTile;
		if (w->waitTicksMissile > Constant_FigureProperties[w->type].missileFrequency) {
			w->waitTicksMissile = 0;
			if (FigureAction_CombatEnemy_getMissileTarget(walkerId, 10, Data_CityInfo.numSoldiersInCity < 4, &xTile, &yTile)) {
				w->attackGraphicOffset = 1;
				w->direction = Routing_getDirectionForMissileShooter(w->x, w->y, xTile, yTile);
			} else {
				w->attackGraphicOffset = 0;
			}
		}
		if (w->attackGraphicOffset) {
			int missileType;
			switch (f->enemyType) {
				case EnemyType_4_Goth:
				case EnemyType_5_Pergamum:
				case EnemyType_9_Egyptian:
				case EnemyType_10_Carthaginian:
					missileType = Figure_Arrow;
					break;
				default:
					missileType = Figure_Spear;
					break;
			}
			if (w->attackGraphicOffset == 1) {
				Figure_createMissile(walkerId, w->x, w->y, xTile, yTile, missileType);
				f->missileFired = 6;
			}
			if (missileType == Figure_Arrow) {
				Data_CityInfo.soundShootArrow--;
				if (Data_CityInfo.soundShootArrow <= 0) {
					Data_CityInfo.soundShootArrow = 10;
					Sound_Effects_playChannel(SoundChannel_Arrow);
				}
			}
			w->attackGraphicOffset++;
			if (w->attackGraphicOffset > 100) {
				w->attackGraphicOffset = 0;
			}
		}
	}
}

static void enemyMarching(int walkerId, struct Data_Walker *w, struct Data_Formation *f)
{
	w->waitTicks--;
	if (w->waitTicks <= 0) {
		w->waitTicks = 50;
		w->destinationX = f->destinationX + w->formationPositionX;
		w->destinationY = f->destinationY + w->formationPositionY;
		if (Routing_getGeneralDirection(w->x, w->y, w->destinationX, w->destinationY) == DirFigure_8_AtDestination) {
			w->actionState = FigureActionState_151_EnemyInitial;
			return;
		}
		w->destinationBuildingId = f->destinationBuildingId;
		FigureRoute_remove(walkerId);
	}
	FigureMovement_walkTicks(walkerId, w->speedMultiplier);
	if (w->direction == DirFigure_8_AtDestination ||
		w->direction == DirFigure_9_Reroute ||
		w->direction == DirFigure_10_Lost) {
		w->actionState = FigureActionState_151_EnemyInitial;
	}
}

static void enemyFighting(int walkerId, struct Data_Walker *w, struct Data_Formation *f)
{
	if (!f->recentFight) {
		w->actionState = FigureActionState_151_EnemyInitial;
	}
	if (w->type != Figure_Enemy46_Camel && w->type != Figure_Enemy47_Elephant) {
		if (w->type == Figure_Enemy48_Chariot || w->type == Figure_Enemy52_MountedArcher) {
			Data_CityInfo.soundMarchHorse--;
			if (Data_CityInfo.soundMarchHorse <= 0) {
				Data_CityInfo.soundMarchHorse = 200;
				Sound_Effects_playChannel(SoundChannel_HorseMoving);
			}
		} else {
			Data_CityInfo.soundMarchEnemy--;
			if (Data_CityInfo.soundMarchEnemy <= 0) {
				Data_CityInfo.soundMarchEnemy = 200;
				Sound_Effects_playChannel(SoundChannel_Marching);
			}
		}
	}
	int targetId = w->targetWalkerId;
	if (FigureIsDead(targetId)) {
		w->targetWalkerId = 0;
		targetId = 0;
	}
	if (targetId <= 0) {
		targetId = FigureAction_CombatEnemy_getTarget(w->x, w->y);
		if (targetId) {
			w->destinationX = Data_Walkers[targetId].x;
			w->destinationY = Data_Walkers[targetId].y;
			w->targetWalkerId = targetId;
			w->targetWalkerCreatedSequence = Data_Walkers[targetId].createdSequence;
			Data_Walkers[targetId].targetedByWalkerId = walkerId;
			FigureRoute_remove(walkerId);
		}
	}
	if (targetId > 0) {
		FigureMovement_walkTicks(walkerId, w->speedMultiplier);
		if (w->direction == DirFigure_8_AtDestination) {
			w->destinationX = Data_Walkers[w->targetWalkerId].x;
			w->destinationY = Data_Walkers[w->targetWalkerId].y;
			FigureRoute_remove(walkerId);
		} else if (w->direction == DirFigure_9_Reroute || w->direction == DirFigure_10_Lost) {
			w->actionState = FigureActionState_151_EnemyInitial;
			w->targetWalkerId = 0;
		}
	} else {
		w->actionState = FigureActionState_151_EnemyInitial;
		w->waitTicks = 50;
	}
}

static void FigureAction_enemyCommon(int walkerId, struct Data_Walker *w)
{
	struct Data_Formation *f = &Data_Formations[w->formationId];
	Data_CityInfo.numEnemiesInCity++;
	w->terrainUsage = FigureTerrainUsage_Enemy;
	w->formationPositionX = WalkerActionFormationLayoutPositionX(f->layout, w->indexInFormation);
	w->formationPositionY = WalkerActionFormationLayoutPositionY(f->layout, w->indexInFormation);

	switch (w->actionState) {
		case FigureActionState_150_Attack:
			FigureAction_Common_handleAttack(walkerId);
			break;
		case FigureActionState_149_Corpse:
			FigureAction_Common_handleCorpse(walkerId);
			break;
		case FigureActionState_148_Fleeing:
			w->destinationX = w->sourceX;
			w->destinationY = w->sourceY;
			FigureMovement_walkTicks(walkerId, w->speedMultiplier);
			if (w->direction == DirFigure_8_AtDestination ||
				w->direction == DirFigure_9_Reroute ||
				w->direction == DirFigure_10_Lost) {
				w->state = FigureState_Dead;
			}
			break;
		case FigureActionState_151_EnemyInitial:
			enemyInitial(walkerId, w, f);
			break;
		case FigureActionState_152_EnemyWaiting:
			Figure_updatePositionInTileList(walkerId);
			break;
		case FigureActionState_153_EnemyMarching:
			enemyMarching(walkerId, w, f);
			break;
		case FigureActionState_154_EnemyFighting:
			enemyFighting(walkerId, w, f);
			break;
	}
}

static int getDirection(struct Data_Walker *w)
{
	int dir;
	if (w->actionState == FigureActionState_150_Attack) {
		dir = w->attackDirection;
	} else if (w->direction < 8) {
		dir = w->direction;
	} else {
		dir = w->previousTileDirection;
	}
	WalkerActionNormalizeDirection(dir);
	return dir;
}

static int getDirectionMissile(struct Data_Walker *w, struct Data_Formation *f)
{
	int dir;
	if (w->actionState == FigureActionState_150_Attack) {
		dir = w->attackDirection;
	} else if (f->missileFired || w->direction < 8) {
		dir = w->direction;
	} else {
		dir = w->previousTileDirection;
	}
	WalkerActionNormalizeDirection(dir);
	return dir;
}

void FigureAction_enemy43_Spear(int walkerId)
{
	struct Data_Walker *w = &Data_Walkers[walkerId];
	struct Data_Formation *f = &Data_Formations[w->formationId];
	FigureActionIncreaseGraphicOffset(w, 12);
	w->cartGraphicId = 0;
	w->speedMultiplier = 1;
	FigureAction_enemyCommon(walkerId, w);
	
	int dir = getDirectionMissile(w, f);
	
	w->isEnemyGraphic = 1;
	
	switch (f->enemyType) {
		case EnemyType_5_Pergamum:
		case EnemyType_6_Seleucid:
		case EnemyType_7_Etruscan:
		case EnemyType_8_Greek:
			break;
		default:
			return;
	}
	if (w->actionState == FigureActionState_150_Attack) {
		if (w->attackGraphicOffset >= 12) {
			w->graphicId = 745 + dir + 8 * ((w->attackGraphicOffset - 12) / 2);
		} else {
			w->graphicId = 745 + dir;
		}
	} else if (w->actionState == FigureActionState_151_EnemyInitial) {
		w->graphicId = 697 + dir + 8 * WalkerActionMissileLauncherGraphicOffset(w);
	} else if (w->actionState == FigureActionState_149_Corpse) {
		w->graphicId = 793 + WalkerActionCorpseGraphicOffset(w);
	} else if (w->direction == DirFigure_11_Attack) {
		w->graphicId = 745 + dir + 8 * (w->graphicOffset / 2);
	} else {
		w->graphicId = 601 + dir + 8 * w->graphicOffset;
	}
}

void FigureAction_enemy44_Sword(int walkerId)
{
	struct Data_Walker *w = &Data_Walkers[walkerId];
	struct Data_Formation *f = &Data_Formations[w->formationId];
	FigureActionIncreaseGraphicOffset(w, 12);
	w->cartGraphicId = 0;
	w->speedMultiplier = 1;
	FigureAction_enemyCommon(walkerId, w);
	
	int dir = getDirection(w);
	
	w->isEnemyGraphic = 1;
	
	switch (f->enemyType) {
		case EnemyType_5_Pergamum:
		case EnemyType_6_Seleucid:
		case EnemyType_9_Egyptian:
			break;
		default:
			return;
	}
	if (w->actionState == FigureActionState_150_Attack) {
		if (w->attackGraphicOffset >= 12) {
			w->graphicId = 545 + dir + 8 * ((w->attackGraphicOffset - 12) / 2);
		} else {
			w->graphicId = 545 + dir;
		}
	} else if (w->actionState == FigureActionState_149_Corpse) {
		w->graphicId = 593 + WalkerActionCorpseGraphicOffset(w);
	} else if (w->direction == DirFigure_11_Attack) {
		w->graphicId = 545 + dir + 8 * (w->graphicOffset / 2);
	} else {
		w->graphicId = 449 + dir + 8 * w->graphicOffset;
	}
}

void FigureAction_enemy45_Sword(int walkerId)
{
	struct Data_Walker *w = &Data_Walkers[walkerId];
	struct Data_Formation *f = &Data_Formations[w->formationId];
	FigureActionIncreaseGraphicOffset(w, 12);
	w->cartGraphicId = 0;
	w->speedMultiplier = 1;
	FigureAction_enemyCommon(walkerId, w);
	
	int dir = getDirection(w);
	
	w->isEnemyGraphic = 1;
	
	switch (f->enemyType) {
		case EnemyType_7_Etruscan:
		case EnemyType_8_Greek:
		case EnemyType_10_Carthaginian:
			break;
		default:
			return;
	}
	if (w->actionState == FigureActionState_150_Attack) {
		if (w->attackGraphicOffset >= 12) {
			w->graphicId = 545 + dir + 8 * ((w->attackGraphicOffset - 12) / 2);
		} else {
			w->graphicId = 545 + dir;
		}
	} else if (w->actionState == FigureActionState_149_Corpse) {
		w->graphicId = 593 + WalkerActionCorpseGraphicOffset(w);
	} else if (w->direction == DirFigure_11_Attack) {
		w->graphicId = 545 + dir + 8 * (w->graphicOffset / 2);
	} else {
		w->graphicId = 449 + dir + 8 * w->graphicOffset;
	}
}

void FigureAction_enemy46_Camel(int walkerId)
{
	struct Data_Walker *w = &Data_Walkers[walkerId];
	struct Data_Formation *f = &Data_Formations[w->formationId];
	FigureActionIncreaseGraphicOffset(w, 12);
	w->cartGraphicId = 0;
	w->speedMultiplier = 1;
	FigureAction_enemyCommon(walkerId, w);
	
	int dir = getDirectionMissile(w, f);
	
	w->isEnemyGraphic = 1;
	
	if (w->direction == DirFigure_11_Attack) {
		w->graphicId = 601 + dir + 8 * w->graphicOffset;
	} else if (w->actionState == FigureActionState_150_Attack) {
		w->graphicId = 601 + dir;
	} else if (w->actionState == FigureActionState_151_EnemyInitial) {
		w->graphicId = 697 + dir + 8 * WalkerActionMissileLauncherGraphicOffset(w);
	} else if (w->actionState == FigureActionState_149_Corpse) {
		w->graphicId = 745 + WalkerActionCorpseGraphicOffset(w);
	} else {
		w->graphicId = 601 + dir + 8 * w->graphicOffset;
	}
}

void FigureAction_enemy47_Elephant(int walkerId)
{
	struct Data_Walker *w = &Data_Walkers[walkerId];
	FigureActionIncreaseGraphicOffset(w, 12);
	w->cartGraphicId = 0;
	w->speedMultiplier = 1;
	FigureAction_enemyCommon(walkerId, w);
	
	int dir = getDirection(w);
	
	w->isEnemyGraphic = 1;
	
	if (w->direction == DirFigure_11_Attack || w->actionState == FigureActionState_150_Attack) {
		w->graphicId = 601 + dir + 8 * w->graphicOffset;
	} else if (w->actionState == FigureActionState_149_Corpse) {
		w->graphicId = 705 + WalkerActionCorpseGraphicOffset(w);
	} else {
		w->graphicId = 601 + dir + 8 * w->graphicOffset;
	}
}

void FigureAction_enemy48_Chariot(int walkerId)
{
	struct Data_Walker *w = &Data_Walkers[walkerId];
	FigureActionIncreaseGraphicOffset(w, 12);
	w->cartGraphicId = 0;
	w->speedMultiplier = 3;
	FigureAction_enemyCommon(walkerId, w);
	
	int dir = getDirection(w);
	
	w->isEnemyGraphic = 1;
	
	if (w->direction == DirFigure_11_Attack || w->actionState == FigureActionState_150_Attack) {
		w->graphicId = 697 + dir + 8 * (w->graphicOffset / 2);
	} else if (w->actionState == FigureActionState_149_Corpse) {
		w->graphicId = 745 + WalkerActionCorpseGraphicOffset(w);
	} else {
		w->graphicId = 601 + dir + 8 * w->graphicOffset;
	}
}

void FigureAction_enemy49_FastSword(int walkerId)
{
	struct Data_Walker *w = &Data_Walkers[walkerId];
	struct Data_Formation *f = &Data_Formations[w->formationId];
	FigureActionIncreaseGraphicOffset(w, 12);
	w->cartGraphicId = 0;
	w->speedMultiplier = 2;
	FigureAction_enemyCommon(walkerId, w);
	
	int dir = getDirection(w);
	
	w->isEnemyGraphic = 1;
	
	int attackId, corpseId, normalId;
	if (f->enemyType == EnemyType_0_Barbarian) {
		attackId = 393;
		corpseId = 441;
		normalId = 297;
	} else if (f->enemyType == EnemyType_1_Numidian) {
		attackId = 593;
		corpseId = 641;
		normalId = 449;
	} else if (f->enemyType == EnemyType_4_Goth) {
		attackId = 545;
		corpseId = 593;
		normalId = 449;
	} else {
		return;
	}
	if (w->actionState == FigureActionState_150_Attack) {
		if (w->attackGraphicOffset >= 12) {
			w->graphicId = attackId + dir + 8 * ((w->attackGraphicOffset - 12) / 2);
		} else {
			w->graphicId = attackId + dir;
		}
	} else if (w->actionState == FigureActionState_149_Corpse) {
		w->graphicId = corpseId + WalkerActionCorpseGraphicOffset(w);
	} else if (w->direction == DirFigure_11_Attack) {
		w->graphicId = attackId + dir + 8 * (w->graphicOffset / 2);
	} else {
		w->graphicId = normalId + dir + 8 * w->graphicOffset;
	}
}

void FigureAction_enemy50_Sword(int walkerId)
{
	struct Data_Walker *w = &Data_Walkers[walkerId];
	struct Data_Formation *f = &Data_Formations[w->formationId];
	FigureActionIncreaseGraphicOffset(w, 12);
	w->cartGraphicId = 0;
	w->speedMultiplier = 1;
	FigureAction_enemyCommon(walkerId, w);
	
	int dir = getDirection(w);
	
	w->isEnemyGraphic = 1;
	
	if (f->enemyType != EnemyType_2_Gaul && f->enemyType != EnemyType_3_Celt) {
		return;
	}
	if (w->actionState == FigureActionState_150_Attack) {
		if (w->attackGraphicOffset >= 12) {
			w->graphicId = 545 + dir + 8 * ((w->attackGraphicOffset - 12) / 2);
		} else {
			w->graphicId = 545 + dir;
		}
	} else if (w->actionState == FigureActionState_149_Corpse) {
		w->graphicId = 593 + WalkerActionCorpseGraphicOffset(w);
	} else if (w->direction == DirFigure_11_Attack) {
		w->graphicId = 545 + dir + 8 * (w->graphicOffset / 2);
	} else {
		w->graphicId = 449 + dir + 8 * w->graphicOffset;
	}
}

void FigureAction_enemy51_Spear(int walkerId)
{
	struct Data_Walker *w = &Data_Walkers[walkerId];
	struct Data_Formation *f = &Data_Formations[w->formationId];
	FigureActionIncreaseGraphicOffset(w, 12);
	w->cartGraphicId = 0;
	w->speedMultiplier = 2;
	FigureAction_enemyCommon(walkerId, w);
	
	int dir = getDirectionMissile(w, f);
	
	w->isEnemyGraphic = 1;
	
	if (f->enemyType != EnemyType_1_Numidian) {
		return;
	}
	if (w->actionState == FigureActionState_150_Attack) {
		if (w->attackGraphicOffset >= 12) {
			w->graphicId = 593 + dir + 8 * ((w->attackGraphicOffset - 12) / 2);
		} else {
			w->graphicId = 593 + dir;
		}
	} else if (w->actionState == FigureActionState_151_EnemyInitial) {
		w->graphicId = 545 + dir + 8 * WalkerActionMissileLauncherGraphicOffset(w);
	} else if (w->actionState == FigureActionState_149_Corpse) {
		w->graphicId = 641 + WalkerActionCorpseGraphicOffset(w);
	} else if (w->direction == DirFigure_11_Attack) {
		w->graphicId = 593 + dir + 8 * (w->graphicOffset / 2);
	} else {
		w->graphicId = 449 + dir + 8 * w->graphicOffset;
	}
}

void FigureAction_enemy52_MountedArcher(int walkerId)
{
	struct Data_Walker *w = &Data_Walkers[walkerId];
	struct Data_Formation *f = &Data_Formations[w->formationId];
	FigureActionIncreaseGraphicOffset(w, 12);
	w->cartGraphicId = 0;
	w->speedMultiplier = 3;
	FigureAction_enemyCommon(walkerId, w);
	
	int dir = getDirectionMissile(w, f);
	
	w->isEnemyGraphic = 1;
	
	if (w->direction == DirFigure_11_Attack) {
		w->graphicId = 601 + dir + 8 * w->graphicOffset;
	} else if (w->actionState == FigureActionState_150_Attack) {
		w->graphicId = 601 + dir;
	} else if (w->actionState == FigureActionState_151_EnemyInitial) {
		w->graphicId = 697 + dir + 8 * WalkerActionMissileLauncherGraphicOffset(w);
	} else if (w->actionState == FigureActionState_149_Corpse) {
		w->graphicId = 745 + WalkerActionCorpseGraphicOffset(w);
	} else {
		w->graphicId = 601 + dir + 8 * w->graphicOffset;
	}
}

void FigureAction_enemy53_Axe(int walkerId)
{
	struct Data_Walker *w = &Data_Walkers[walkerId];
	struct Data_Formation *f = &Data_Formations[w->formationId];
	FigureActionIncreaseGraphicOffset(w, 12);
	w->cartGraphicId = 0;
	w->speedMultiplier = 1;
	FigureAction_enemyCommon(walkerId, w);
	
	int dir = getDirection(w);
	
	w->isEnemyGraphic = 1;
	
	if (f->enemyType != EnemyType_2_Gaul) {
		return;
	}
	if (w->actionState == FigureActionState_150_Attack) {
		if (w->attackGraphicOffset >= 12) {
			w->graphicId = 697 + dir + 8 * ((w->attackGraphicOffset - 12) / 2);
		} else {
			w->graphicId = 697 + dir;
		}
	} else if (w->actionState == FigureActionState_149_Corpse) {
		w->graphicId = 745 + WalkerActionCorpseGraphicOffset(w);
	} else if (w->direction == DirFigure_11_Attack) {
		w->graphicId = 697 + dir + 8 * (w->graphicOffset / 2);
	} else {
		w->graphicId = 601 + dir + 8 * w->graphicOffset;
	}
}

void FigureAction_enemy54_Gladiator(int walkerId)
{
	struct Data_Walker *w = &Data_Walkers[walkerId];
	w->terrainUsage = FigureTerrainUsage_Any;
	w->useCrossCountry = 0;
	FigureActionIncreaseGraphicOffset(w, 12);
	if (Data_Event.gladiatorRevolt.state == SpecialEvent_Finished) {
		// end of gladiator revolt: kill gladiators
		if (w->actionState != FigureActionState_149_Corpse) {
			w->actionState = FigureActionState_149_Corpse;
			w->waitTicks = 0;
			w->direction = 0;
		}
	}
	switch (w->actionState) {
		case FigureActionState_150_Attack:
			FigureAction_Common_handleAttack(walkerId);
			FigureActionIncreaseGraphicOffset(w, 16);
			break;
		case FigureActionState_149_Corpse:
			FigureAction_Common_handleCorpse(walkerId);
			break;
		case FigureActionState_158_NativeCreated:
			w->graphicOffset = 0;
			w->waitTicks++;
			if (w->waitTicks > 10 + (walkerId & 3)) {
				w->waitTicks = 0;
				w->actionState = FigureActionState_159_NativeAttacking;
				int xTile, yTile;
				int buildingId = Formation_Rioter_getTargetBuilding(&xTile, &yTile);
				if (buildingId) {
					w->destinationX = xTile;
					w->destinationY = yTile;
					w->destinationBuildingId = buildingId;
					FigureRoute_remove(walkerId);
				} else {
					w->state = FigureState_Dead;
				}
			}
			break;
		case FigureActionState_159_NativeAttacking:
			Data_CityInfo.numAttackingNativesInCity = 10;
			w->terrainUsage = FigureTerrainUsage_Enemy;
			FigureMovement_walkTicks(walkerId, 1);
			if (w->direction == DirFigure_8_AtDestination ||
				w->direction == DirFigure_9_Reroute ||
				w->direction == DirFigure_10_Lost) {
				w->actionState = FigureActionState_158_NativeCreated;
			}
			break;
	}
	int dir;
	if (w->actionState == FigureActionState_150_Attack || w->direction == DirFigure_11_Attack) {
		dir = w->attackDirection;
	} else if (w->direction < 8) {
		dir = w->direction;
	} else {
		dir = w->previousTileDirection;
	}
	WalkerActionNormalizeDirection(dir);

	if (w->actionState == FigureActionState_150_Attack || w->direction == DirFigure_11_Attack) {
		w->graphicId = GraphicId(ID_Graphic_Figure_Gladiator) + dir + 104 + 8 * (w->graphicOffset / 2);
	} else if (w->actionState == FigureActionState_149_Corpse) {
		w->graphicId = GraphicId(ID_Graphic_Figure_Gladiator) + 96 + WalkerActionCorpseGraphicOffset(w);
	} else {
		w->graphicId = GraphicId(ID_Graphic_Figure_Gladiator) + dir + 8 * w->graphicOffset;
	}
}

void FigureAction_enemyCaesarLegionary(int walkerId)
{
	struct Data_Walker *w = &Data_Walkers[walkerId];
	struct Data_Formation *f = &Data_Formations[w->formationId];
	Data_CityInfo.numImperialSoldiersInCity++;
	FigureActionIncreaseGraphicOffset(w, 12);
	w->cartGraphicId = 0;
	w->speedMultiplier = 1;
	FigureAction_enemyCommon(walkerId, w);
	
	int dir = getDirection(w);
	
	if (w->direction == DirFigure_11_Attack) {
		w->graphicId = GraphicId(ID_Graphic_Figure_CaesarLegionary) + dir +
			8 * ((w->attackGraphicOffset - 12) / 2);
	}
	switch (w->actionState) {
		case FigureActionState_150_Attack:
			if (w->attackGraphicOffset >= 12) {
				w->graphicId = GraphicId(ID_Graphic_Figure_CaesarLegionary) + dir +
					8 * ((w->attackGraphicOffset - 12) / 2);
			} else {
				w->graphicId = GraphicId(ID_Graphic_Figure_CaesarLegionary) + dir;
			}
			break;
		case FigureActionState_149_Corpse:
			w->graphicId = GraphicId(ID_Graphic_Figure_CaesarLegionary) +
				WalkerActionCorpseGraphicOffset(w) + 152;
			break;
		case FigureActionState_84_SoldierAtStandard:
			if (f->isHalted && f->layout == FormationLayout_Tortoise && f->missileAttackTimeout) {
				w->graphicId = GraphicId(ID_Graphic_Figure_FortLegionary) + dir + 144;
			} else {
				w->graphicId = GraphicId(ID_Graphic_Figure_FortLegionary) + dir;
			}
			break;
		default:
			w->graphicId = GraphicId(ID_Graphic_Figure_CaesarLegionary) + 48 + dir + 8 * w->graphicOffset;
			break;
	}
}

int FigureAction_HerdEnemy_moveFormationTo(int formationId, int x, int y, int *xTile, int *yTile)
{
	struct Data_Formation *f = &Data_Formations[formationId];
	int baseOffset = GridOffset(
		WalkerActionFormationLayoutPositionX(f->layout, 0),
		WalkerActionFormationLayoutPositionY(f->layout, 0));
	int walkerOffsets[50];
	walkerOffsets[0] = 0;
	for (int i = 1; i < f->numFigures; i++) {
		walkerOffsets[i] = GridOffset(
			WalkerActionFormationLayoutPositionX(f->layout, i),
			WalkerActionFormationLayoutPositionY(f->layout, i)) - baseOffset;
	}
	Routing_canTravelOverLandNonCitizen(x, y, -1, -1, 0, 600);
	for (int r = 0; r <= 10; r++) {
		int xMin = x - r;
		int yMin = y - r;
		int xMax = x + r;
		int yMax = y + r;
		Bound2ToMap(xMin, yMin, xMax, yMax);
		for (int yy = yMin; yy <= yMax; yy++) {
			for (int xx = xMin; xx <= xMax; xx++) {
				int canMove = 1;
				for (int w = 0; w < f->numFigures; w++) {
					int gridOffset = GridOffset(xx, yy) + walkerOffsets[w];
					if (Data_Grid_terrain[gridOffset] & Terrain_1237) {
						canMove = 0;
						break;
					}
					if (Data_Grid_routingDistance[gridOffset] <= 0) {
						canMove = 0;
						break;
					}
					if (Data_Grid_figureIds[gridOffset] &&
						Data_Walkers[Data_Grid_figureIds[gridOffset]].formationId != formationId) {
						canMove = 0;
						break;
					}
				}
				if (canMove) {
					*xTile = xx;
					*yTile = yy;
					return 1;
				}
			}
		}
	}
	return 0;
}
