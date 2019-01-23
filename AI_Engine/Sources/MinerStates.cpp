#include "pch.h"
#include "..\Headers\MinerStates.h"
#include "GameScene.h"
#include "Wife.h"

using namespace std;

void MiningForGold::Execute(Miner * miner)
{
	//@Mining increases gold, and makes the player hungry, thirsty and tired
	miner->m_gold++;
	
	miner->m_tiredness++;
	miner->m_hunger++;
	miner->m_thirstiness++;


	//@Log (Also reset length)
	miner->m_text = L"Mining!";

}

void BankingGold::Execute(Miner * miner)
{
	//@Deposits gold in the bank
	miner->m_bankedGold += miner->m_gold;
	miner->m_gold = 0;

	//@Log (Also reset length)
	miner->m_text = L"Banking!";
}

void Resting::Execute(Miner * miner)
{
	//Every execution
	if (miner->m_tiredness > 0) miner->m_tiredness--;

	//@Log (Also reset length)
	miner->m_text = L"Resting!";
}

void Drinking::Execute(Miner * miner)
{
	//@If we got here, we have enough money to buy a drink
	int moneyToFill = 5;
	if (miner->m_gold > 0) {
		moneyToFill -= miner->m_gold;
		miner->m_gold = 0;
	}
	//Left over gold?
	miner->m_bankedGold -= moneyToFill;//MoneyToFill is 0

	miner->m_thirstiness = 0;
	//@Log (Also reset length)
	miner->m_text = L"Drinking!";
}

void HavingMeal::Execute(Miner * miner)
{
	//Iterate through UI elements to find possible clicked UI Button
	GameScene* gs = GameScene::GetInstance();
	for (unsigned int i = 0; i < gs->m_entities.size(); i++) {
		//Look for entities which can be mouse pressed
		Wife * wife = dynamic_cast<Wife *>(gs->m_entities[i]);
		if (wife) {
			//@Ideally always gets wife, otherwise, game over?
			
	
		}
	}
	//@Log (Also reset length)
	miner->m_text = L"Having Meal!";
}

