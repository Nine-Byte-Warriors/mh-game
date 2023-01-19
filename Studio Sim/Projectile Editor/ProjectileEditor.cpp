#include "stdafx.h"
#include "ProjectileEditor.h"
#include "FileLoading.h"

#include "Graphics.h"	// TODO: Remove this as shouldn't need this here!

#if _DEBUG
#include <imgui/imgui.h>
#endif // _DEBUG

#define DEFAULT_PATTERN_PATH "Resources\\Patterns\\"

ProjectileEditor::ProjectileEditor() :
	m_sSelectedFile("ProjectilePattern.json"),
	m_sFileContent(""),
	m_sFilePath(""),
	m_iProjectileCount(0),
	m_bMidPosSet(false),
	m_vSpawnPosition(Vector2f())
{
	//m_vecProjectileManager.push_back(std::make_shared<ProjectileManager>());
	// TODO: Initialise images files for sprites of projectiles.
}

void ProjectileEditor::Initialise(const Graphics& gfx, ConstantBuffer<Matrices>& mat)
{
	for(std::shared_ptr<ProjectileManager> pProMan : m_vecProjectileManager)
		pProMan->Initialize(gfx, mat);
}

void ProjectileEditor::Update(const float dt)
{
	for (std::shared_ptr <ProjectileManager> pProMan : m_vecProjectileManager)
		pProMan->Update(dt);
}

void ProjectileEditor::Draw(ID3D11DeviceContext* context, XMMATRIX matrix)
{
	for (std::shared_ptr <ProjectileManager> pProMan : m_vecProjectileManager)
		pProMan->Draw(context, matrix);
}

#if _DEBUG
void ProjectileEditor::SpawnEditorWindow(const Graphics& gfx, ConstantBuffer<Matrices>& mat)
{
	if (ImGui::Begin("Projectile Editor", FALSE, ImGuiWindowFlags_AlwaysAutoResize + ImGuiWindowFlags_HorizontalScrollbar))
	{
		LoadPattern();
		SavePattern();

		ImGui::Separator();
		SpawnPosition(Vector2f(gfx.GetWidth(), gfx.GetHeight()));

		ImGui::Separator();
		ShowPattern();

		TestButtons(gfx, mat);
	}
	ImGui::End();
}

void ProjectileEditor::LoadPattern()
{
	static char loadFileName[128] = "";
	
	bool bLoadButton = ImGui::Button("Load Pattern");
	ImGui::InputTextWithHint("##PatternLoadFile", "Load File Name", loadFileName, IM_ARRAYSIZE(loadFileName));
	ImGui::Text(m_sSelectedFile.c_str());

	if (!bLoadButton)
		return;
	
	if (FileLoading::OpenFileExplorer(m_sSelectedFile, m_sFilePath))
		JsonLoading::LoadJson(m_vecManagers, m_sFilePath);
	else
		m_sSelectedFile = "Open File Failed";
}

void ProjectileEditor::SavePattern()
{
	static char saveFileName[128] = "";
	m_bSaveButton = ImGui::Button("Save Pattern");
	ImGui::InputTextWithHint("##PatternSaveFile", "New Save File Name", saveFileName, IM_ARRAYSIZE(saveFileName));
	ImGui::Text(m_sSelectedFile.c_str());
	
	if (!m_bSaveButton)
		return;
	
	if (m_vecManagers.size() < 1)
		return;

	if (FileLoading::OpenFileExplorer(m_sSelectedFile, m_sFilePath))
		JsonLoading::SaveJson(m_vecManagers, m_sFilePath);
}

void ProjectileEditor::SpawnPosition(Vector2f vWinMax)
{
	if (!m_bMidPosSet)
	{
		m_vSpawnPosition = Vector2f(vWinMax.x / 2, vWinMax.y / 2);
		m_bMidPosSet = true;
	}

	ImGui::Text("Spawn Position");
	ImGui::DragFloat("SpawnPosition-X", &m_vSpawnPosition.x, 1.0f, 0.0f, vWinMax.x, "%.1f");
	ImGui::DragFloat("SpawnPosition-Y", &m_vSpawnPosition.y, 1.0f, 0.0f, vWinMax.y, "%.1f");
}

void ProjectileEditor::ShowPattern()
{
	std::string msg;
	
	if (m_vecManagers.size() < 1)
		m_vecManagers.push_back(CreateDefaultManager());
	
	if(ImGui::Button("Add Pattern"))
		m_vecManagers.push_back(CreateDefaultManager());

	for (int iManIndex = 0; iManIndex < m_vecManagers.size(); iManIndex++)
	{
		std::string sManagerTitle = std::string("Manager #").append(std::to_string(iManIndex));
		if(ImGui::CollapsingHeader(sManagerTitle.c_str()))
		{
			msg = "Pattern " + m_vecManagers[iManIndex].m_sName;
			ImGui::Text(msg.c_str());

			msg = "ID: " + m_vecManagers[iManIndex].m_sID;
			ImGui::Text(msg.c_str());

			ImGui::Text(std::string("Count: ").append(std::to_string(m_vecManagers[iManIndex].m_vecProjectiles.size())).c_str());
			if (ImGui::Button("Add Projectile"))
				m_vecManagers[iManIndex].m_vecProjectiles.push_back(ProjectileData::ProjectileJSON());

			ImGui::Text(std::string("Delay: ").append(std::to_string(m_vecManagers[iManIndex].m_fDelay)).c_str());
			ImGui::SliderFloat(
				std::string("Delay##").append(std::to_string(iManIndex)).c_str(),
				&m_vecManagers[iManIndex].m_fDelay,
				0,
				100
			);

			std::vector <ProjectileData::ProjectileJSON> vecProjectiles = m_vecManagers[iManIndex].m_vecProjectiles;
			for (int iProIndex = 0; iProIndex < vecProjectiles.size(); iProIndex++)
			{
				std::string sProjTitle = "Projectile";
				sProjTitle
					.append(std::to_string(iProIndex))
					.append("##Man")
					.append(std::to_string(iManIndex));
				if (ImGui::TreeNode(sProjTitle.c_str()))
				{
					ImGui::Text("Spawn Position");
					ImGui::SliderFloat(
						std::string("X##Man")
							.append(std::to_string(iManIndex))
							.append("Pro")
							.append(std::to_string(iProIndex))
							.c_str(),
						&m_vecManagers[iManIndex].m_vecProjectiles[iProIndex].m_fX,
						-100.0f, 100.0f);
					ImGui::SliderFloat(
						std::string("Y##Man")
							.append(std::to_string(iManIndex))
							.append("Pro")
							.append(std::to_string(iProIndex))
							.c_str(),
						&m_vecManagers[iManIndex].m_vecProjectiles[iProIndex].m_fY,
						-100.0f, 100.0f);

					ImGui::Separator();

					msg = "LifeTime: " + std::to_string(m_vecManagers[iManIndex].m_vecProjectiles[iProIndex].m_fLifeTime);
					ImGui::Text(msg.c_str());
					ImGui::SliderFloat(
						std::string("LifeTime##Man")
							.append(std::to_string(iManIndex))
							.append("Pro")
							.append(std::to_string(iProIndex))
							.c_str(),
						&m_vecManagers[iManIndex].m_vecProjectiles[iProIndex].m_fLifeTime,
						0.0f, 1000.0f, "%0.2f");

					msg = "Speed: " + std::to_string(m_vecManagers[iManIndex].m_vecProjectiles[iProIndex].m_fSpeed);
					ImGui::Text(msg.c_str());
					ImGui::SliderFloat(
						std::string("Speed##Man")
							.append(std::to_string(iManIndex))
							.append("Pro")
							.append(std::to_string(iProIndex))
							.c_str(), 
						&m_vecManagers[iManIndex].m_vecProjectiles[iProIndex].m_fSpeed,
						0.0f, 100.0f, "%0.2f");


					msg = "Heading Angle: " + std::to_string(m_vecManagers[iManIndex].m_vecProjectiles[iProIndex].m_fAngle);
					ImGui::SliderAngle(
						std::string("Angle##Man")
							.append(std::to_string(iManIndex))
							.append("Pro")
							.append(std::to_string(iProIndex))
							.c_str(),
						&m_vecManagers[iManIndex].m_vecProjectiles[iProIndex].m_fAngle);

					msg = "Traveling Angle Adjustment: " + std::to_string(m_vecManagers[iManIndex].m_vecProjectiles[iProIndex].m_fWaveAngle);
					ImGui::DragFloat(
						std::string("Wave##Man")
							.append(std::to_string(iManIndex))
							.append("Pro")
							.append(std::to_string(iProIndex))
							.c_str(),
						&m_vecManagers[iManIndex].m_vecProjectiles[iProIndex].m_fWaveAngle,
						0.1f, -360.0f, 360.0f, "%.2f");

					ImGui::TreePop();
				}
			}
		}
	}
}

void ProjectileEditor::TestButtons(const Graphics& gfx, ConstantBuffer<Matrices>& mat)
{
	bool bFire = ImGui::Button("Test Fire");

	if (!bFire)
		return;

	m_vecProjectileManager.clear();

	for (int i = 0; i < m_vecManagers.size(); i++)
	{
		std::shared_ptr <ProjectileManager> pManager = std::make_shared<ProjectileManager>(); 
		pManager->SetDelay(m_vecManagers[i].m_fDelay);
		pManager->SetProjectilePool(CreateProjectilePool(m_vecManagers[i].m_vecProjectiles));
		pManager->InitialiseFromFile(gfx, mat, m_vecManagers[i].m_sImagePath);

		m_vecProjectileManager.push_back(std::move(pManager)); 
	}	

	SpawnPattern();
}

std::vector<std::shared_ptr<Projectile>> ProjectileEditor::CreateProjectilePool(std::vector<ProjectileData::ProjectileJSON> vecProjectileJsons)
{
	std::vector<std::shared_ptr<Projectile>> vecProjectilePool;

	for (ProjectileData::ProjectileJSON pJson : vecProjectileJsons)
	{
		std::shared_ptr<Projectile> pProjectile = std::make_shared<Projectile>(pJson.m_fSpeed, pJson.m_fLifeTime);
		pProjectile->SetDirection(Vector2f(pJson.m_fAngle));
		pProjectile->SetOffSet(Vector2f(pJson.m_fX, pJson.m_fY));
		if (pJson.m_fWaveAngle != 0.0f)
			pProjectile->SetWaveAngle(pJson.m_fWaveAngle);

		vecProjectilePool.push_back(std::move(pProjectile));
	}

	return vecProjectilePool;
}

void ProjectileEditor::SaveProjectile()
{
	JsonLoading::SaveJson(m_vecProjectiles, m_sFilePath);
}

void ProjectileEditor::SpawnPattern()
{
	for (std::shared_ptr<ProjectileManager> manager : m_vecProjectileManager)
		manager->SpawnProjectiles(m_vSpawnPosition);
}

ProjectileData::ManagerJSON ProjectileEditor::CreateDefaultManager()
{
	ProjectileData::ManagerJSON manager;
	manager.m_sID = "ID";
	manager.m_sName = "Default Pattern";
	manager.m_sImagePath = "Resources\\Textures\\Base_Projectile.png";
	manager.m_fDelay = 0.0f;
	manager.m_iCount = 1;

	ProjectileData::ProjectileJSON blankProjectile;
	blankProjectile.m_fSpeed = 10.0f;
	blankProjectile.m_fLifeTime = 100;
	blankProjectile.m_fAngle = 0.0f;
	blankProjectile.m_fX = 0.0f;
	blankProjectile.m_fY = 0.0f;
	blankProjectile.m_fWaveAngle = 0.0f;

	manager.m_vecProjectiles.push_back(blankProjectile);
	return manager;
}

void ProjectileEditor::AddToEvent() noexcept
{
	/*EventSystem::Instance()->AddClient(EVENTID::PlayerPosition, this);
	EventSystem::Instance()->AddClient(EVENTID::TargetPosition, this);
	EventSystem::Instance()->AddClient(EVENTID::PlayerFire, this);*/
}

void ProjectileEditor::HandleEvent(Event* event)
{
	switch (event->GetEventID())
	{
	/*case EVENTID::PlayerPosition:
		m_vSpawnPosition = *static_cast<Vector2f*>(event->GetData());
		break;
	case EVENTID::TargetPosition:
		m_vTargetPosition = *static_cast<Vector2f*>(event->GetData());
		break;
	case EVENTID::PlayerFire:
		SpawnProjectile();
		break;*/
	default:
		break;
	}
}
#endif // _DEBUG