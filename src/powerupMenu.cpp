#include "powerupMenu.hpp"

#include <memory>
#include <set>

#include "raylib.h"

#include "gameState.hpp"
#include "globalDataWrapper.hpp"
#include "settingsManager.hpp"
#include "assetManager.hpp"

#include "constants.hpp"

PowerupMenu::PowerupMenu(std::shared_ptr<GlobalDataWrapper> global_data, std::shared_ptr<AssetManager> assets, 
		std::shared_ptr<SettingsManager> settings, std::shared_ptr<GameState> game_state):
	GlobalData(global_data),
	Settings(settings),
	Assets(assets),
	m_GameState(game_state)
{
	PowerupMenu::GenerateList();
}

void PowerupMenu::Draw(RenderTexture2D& canvas) const noexcept
{
	static constexpr float mid = REFERENCE_WIDTH / 2.0f;

	int x1 = mid - MeasureText(this->SelectionList[0].DisplayName.c_str(), 24) / 2.0f;
	int x2 = mid - MeasureText(this->SelectionList[1].DisplayName.c_str(), 24) / 2.0f;
	int x3 = mid - MeasureText(this->SelectionList[2].DisplayName.c_str(), 24) / 2.0f;

	int x4 = mid - MeasureText(this->SelectionList[0].Description, 21) / 2.0f;
	int x5 = mid - MeasureText(this->SelectionList[1].Description, 21) / 2.0f;
	int x6 = mid - MeasureText(this->SelectionList[2].Description, 21) / 2.0f;

	BeginTextureMode(canvas);
		ClearBackground(BLACK);

		DrawText("Select A Powerup", 452.5, 80, 42, VIOLET);

		DrawText(this->SelectionList[0].DisplayName.c_str(), x1, 260, 24, GOLD);
		DrawText(this->SelectionList[0].Description, x4, 294, 21, GRAY);

		DrawText(this->SelectionList[1].DisplayName.c_str(), x2, 360, 24, GOLD);
		DrawText(this->SelectionList[1].Description, x5, 394, 21, GRAY);

		DrawText(this->SelectionList[2].DisplayName.c_str(), x3, 460, 24, GOLD);
		DrawText(this->SelectionList[2].Description, x6, 494, 21, GRAY);

		DrawText("[TAB] Toggle Powerup Menu, [Enter] LET'S GO GAMBLING!!!", 326, 580, 21, LIGHTGRAY);

		DrawText(this->GlobalData->CachedStrings.at(CachedString::UnclaimedPowerups).c_str(), 50, 670, 21, LIGHTGRAY);
	EndTextureMode();
}

void PowerupMenu::GenerateList() noexcept
{
	std::set<Powerup> powerup_set = {};
	while (true)
	{
		Powerup powerup = static_cast<Powerup>( GetRandomValue(0, ( static_cast<int>(Powerup::COUNT) ) - 1) );
		
		if (!powerup_set.count(powerup))
			powerup_set.insert(powerup);

		if (powerup_set.size() == 3)
			break;
	}

	this->SelectionList.clear();
	
	for (auto const &powerup : powerup_set)
	{
		int i = this->SelectionList.size();

		this->SelectionList.emplace_back(
				powerup,
				"[" + std::to_string(i + 1) + "] " + this->PowerupNames[static_cast<size_t>(powerup)],
				this->PowerupDescriptions[static_cast<size_t>(powerup)]
				);
	}
}

void PowerupMenu::HandleInput() noexcept
{
	if (IsKeyPressed(KEY_ONE) || this->Gamble)
		PowerupMenu::ApplyPowerup(this->SelectionList[0].Powerup);
	else if (IsKeyPressed(KEY_TWO))
		PowerupMenu::ApplyPowerup(this->SelectionList[1].Powerup);
	else if (IsKeyPressed(KEY_THREE))
		PowerupMenu::ApplyPowerup(this->SelectionList[2].Powerup);

	if (IsKeyPressed(KEY_TAB))
	{
		this->Gamble = false;
		this->GlobalData->ActiveState = State::Game;
	}

	if (IsKeyPressed(KEY_ENTER) && this->Gamble == false)
		this->Gamble = true;
	else if (IsKeyPressed(KEY_ENTER) && this->Gamble == true)
		this->Gamble = false;
}

void PowerupMenu::ApplyPowerup(Powerup powerup) noexcept
{
	auto handler_function = this->ApplyHandles[static_cast<size_t>(powerup)];

	(this->*handler_function)();

	PowerupMenu::GenerateList();

	if (!this->Settings->Data.at(SettingKey::UnlimitedPowerups))
	{
		this->m_GameState->UnclaimedPowerups--;
		this->GlobalData->CachedStrings[CachedString::UnclaimedPowerups] = "Unclaimed Powerups: " + std::to_string(this->m_GameState->UnclaimedPowerups);
	}

	if (this->m_GameState->UnclaimedPowerups == 0 && !this->Settings->Data.at(SettingKey::UnlimitedPowerups))
	{
		this->GlobalData->ActiveState = State::Game;
		this->Gamble = false;
	}
}


void PowerupMenu::ApplyEffect(const Effect effect, const Event event, const unsigned int duration) noexcept
{
	this->m_GameState->Effects.insert(effect);

	if (this->m_GameState->Events.count(event))
		this->m_GameState->Events[event] += duration;
	else
		this->m_GameState->Events[event] = this->m_GameState->Ticks + duration;
}

void PowerupMenu::ApplyMilk() noexcept
{
	PowerupMenu::ApplyEffect(Effect::Milk, Event::MilkExpire, SECONDS_TO_TICKS(180));

	this->m_GameState->Effects.erase(Effect::Aussie);
	this->m_GameState->Effects.erase(Effect::Drunk);
	this->m_GameState->Effects.erase(Effect::Trapped);

	this->m_GameState->Effects.erase(Effect::Poison);

	this->m_GameState->Events.erase(Event::PoisonTick);
	this->m_GameState->Events.erase(Event::PoisonExpire);
}

void PowerupMenu::ApplyGreenbull() noexcept
{
	PowerupMenu::ApplyEffect(Effect::Greenbull, Event::GreenbullExpire, SECONDS_TO_TICKS(120));
}

void PowerupMenu::ApplyMagnetism() noexcept
{
	PowerupMenu::ApplyEffect(Effect::Magnetism, Event::MagnetismExpire, SECONDS_TO_TICKS(240));
}

void PowerupMenu::ApplyAura() noexcept
{
	if (this->m_GameState->Attributes.count(Attribute::AuraSize))
	{
		if (this->m_GameState->Attributes[Attribute::AuraSize] + 50 < REFERENCE_HEIGHT)
			this->m_GameState->Attributes[Attribute::AuraSize] += 50;
		else
			this->m_GameState->Attributes[Attribute::AuraSize] = REFERENCE_HEIGHT;

		this->m_GameState->Attributes[Attribute::AuraDamage] += 5;

		if (this->m_GameState->Attributes[Attribute::AuraCooldown] - 25 > TICK_RATE / 4)
			this->m_GameState->Attributes[Attribute::AuraCooldown] -= 25;
		else
			this->m_GameState->Attributes[Attribute::AuraCooldown] = TICK_RATE / 4;
	}
	else
	{
		this->m_GameState->Attributes[Attribute::AuraSize] = 100;
		this->m_GameState->Attributes[Attribute::AuraDamage] = 5;
		this->m_GameState->Attributes[Attribute::AuraCooldown] = SECONDS_TO_TICKS(2);
	}

	this->m_GameState->Events[Event::AuraTick] = this->m_GameState->Ticks + this->m_GameState->Attributes.at(Attribute::AuraCooldown);
}

void PowerupMenu::ApplyBuckshot() noexcept
{
	this->m_GameState->Attributes[Attribute::Buckshot] += 2;

	if (this->m_GameState->Attributes[Attribute::BuckshotSpread] - 0.02f > 5 * TO_RAD)
		this->m_GameState->Attributes[Attribute::BuckshotSpread] -= 0.02f;
	else
		this->m_GameState->Attributes[Attribute::BuckshotSpread] = 5 * TO_RAD;
}

void PowerupMenu::ApplyProjectile() noexcept
{
	if (this->m_GameState->Attributes[Attribute::BulletCooldown] - 25 > 80)
		this->m_GameState->Attributes[Attribute::BulletCooldown] -= 25;
	else
		this->m_GameState->Attributes[Attribute::BulletCooldown] = 80;

	this->m_GameState->Attributes[Attribute::BulletDamage] += 5;
	this->m_GameState->Attributes[Attribute::BulletSpeed] += 100;
}

void PowerupMenu::ApplyLazer() noexcept
{
	if (this->m_GameState->Attributes[Attribute::LazerCooldown] - 50 > 60)
		this->m_GameState->Attributes[Attribute::LazerCooldown] -= 50;
	else
		this->m_GameState->Attributes[Attribute::LazerCooldown] = 60;

	this->m_GameState->Attributes[Attribute::LazerDamage] += 10;
	this->m_GameState->Attributes[Attribute::LazerScale] += 0.5;
	this->m_GameState->Attributes[Attribute::LazerSpeed] += 10;
}

void PowerupMenu::ApplyLifeSteal() noexcept
{
	if (this->m_GameState->Effects.count(Effect::LifeSteal))
		this->m_GameState->Attributes[Attribute::LifeStealMultiplier] += 0.2;
	else
	{
		this->m_GameState->Attributes[Attribute::LifeStealMultiplier] = 0.1;
		this->m_GameState->Effects.insert(Effect::LifeSteal);
	}
}

void PowerupMenu::ApplyPlotArmour() noexcept
{
	if (this->m_GameState->Events.count(Event::IncreasePlotArmour))
		this->m_GameState->Events[Event::IncreasePlotArmour]++;
	else
		this->m_GameState->Events[Event::IncreasePlotArmour] = 1;
}

void PowerupMenu::ApplySpeedBoots() noexcept
{
	if (this->m_GameState->Events.count(Event::IncreasePlayerSpeed))
		this->m_GameState->Events[Event::IncreasePlayerSpeed]++;
	else
		this->m_GameState->Events[Event::IncreasePlayerSpeed] = 1;
}

void PowerupMenu::ApplyBabyOil() noexcept
{
	this->m_GameState->Attributes[Attribute::SlideSpeed] *= 1.2f;
}
