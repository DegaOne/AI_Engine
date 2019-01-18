#include "pch.h"
#include "Miner.h"
#include "MinerSM.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace Microsoft::WRL;

Miner::Miner()
{
	//@Note: default constructor wont set attachedCharacter and might cause crashes
	m_text = L"Default log!";
	m_stateMachine = new MinerSM(this);
	m_gold = 0;
	m_bankedGold = 0;
	m_thirstiness = 0;
	m_hunger = 0;
	m_tiredness = 0;
	m_startMoving = true;
	m_isMoving = false;
	m_fracT = 0;
}

Miner::~Miner()
{
	bool debug;
	delete m_stateMachine;
	m_stateMachine = nullptr;
	m_font.reset();
}

void Miner::Initialize(ID3D11Device1 * device, DirectX::SimpleMath::Vector2 screenPos, float layerDepth)
{
	//Original idle texture(Not animated)
	ComPtr<ID3D11Resource> resource;
	//Fill m_texture
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"Textures/mineridle.dds",
			resource.GetAddressOf(),
			m_texture.ReleaseAndGetAddressOf())
	);
	ComPtr<ID3D11Texture2D> tex;
	DX::ThrowIfFailed(resource.As(&tex));

	CD3D11_TEXTURE2D_DESC texDesc;
	tex->GetDesc(&texDesc);

	m_origin.x = float(texDesc.Width / 2);
	m_origin.y = float(texDesc.Height / 2);
	m_screenPos = screenPos;

	//Animated Textures in vector (Ruled by MinerSM enum)
	//Ideally the resurce width and height are the same
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_animTexture;
	DX::ThrowIfFailed(CreateDDSTextureFromFile(device, L"Textures/minermining.dds",
		nullptr, m_animTexture.ReleaseAndGetAddressOf()));
	m_animatedTextures.push_back(m_animTexture);

	DX::ThrowIfFailed(CreateDDSTextureFromFile(device, L"Textures/minerbanking.dds",
		nullptr, m_animTexture.ReleaseAndGetAddressOf()));
	m_animatedTextures.push_back(m_animTexture);

	DX::ThrowIfFailed(CreateDDSTextureFromFile(device, L"Textures/minerresting.dds",
		nullptr, m_animTexture.ReleaseAndGetAddressOf()));
	m_animatedTextures.push_back(m_animTexture);

	DX::ThrowIfFailed(CreateDDSTextureFromFile(device, L"Textures/minerdrinking.dds",
		nullptr, m_animTexture.ReleaseAndGetAddressOf()));
	m_animatedTextures.push_back(m_animTexture);

	DX::ThrowIfFailed(CreateDDSTextureFromFile(device, L"Textures/minerhavingmeal.dds",
		nullptr, m_animTexture.ReleaseAndGetAddressOf()));
	m_animatedTextures.push_back(m_animTexture);

	//All defaults, last argument is layerDepth
	m_animator = std::make_unique<AnimatedTexture>(XMFLOAT2(0, 0), 0.0f, 1.0f, layerDepth);
	m_animator->Load(m_animatedTextures[0].Get(), 2, 2);

	//Font
	m_font = std::make_unique<SpriteFont>(device, L"Textures/myfile.spritefont");
}

void Miner::Update(float elapsedTime)
{
	AnimatedEntity::Update(elapsedTime);
	//State transitions, state execution
	m_stateMachine->Update(elapsedTime);

	//Movement based on state
	if (m_startMoving) {
		m_startMoving = false;
		m_isMoving = true;
		m_fracT = 0;
		//@Load walking animation
	}

	//Handles state transitions
	if (m_isMoving)
	{
		m_fracT += elapsedTime;
		switch (m_stateMachine->m_charState)
		{
		case MINING_GOLD:
			//Lerp to goal position
			m_screenPos = Vector2::Lerp(m_screenPos, Vector2(1250, 550), m_fracT);
			break;
		case BANKING_GOLD:
			m_screenPos = Vector2::Lerp(m_screenPos, Vector2(1000, 350), m_fracT);
			break;
		case RESTING:
			m_screenPos = Vector2::Lerp(m_screenPos, Vector2(750, 550), m_fracT);
			break;
		case DRINKING:
			m_screenPos = Vector2::Lerp(m_screenPos, Vector2( 1000, 750), m_fracT);
			break;
		case HAVING_MEAL:
			break;
		}
		//Check that lerp ended
		if (m_fracT >= 1) {
			m_isMoving = false;
			//@Load right animation based on state, here
		}
	}
}

void Miner::Render(DirectX::SpriteBatch * spriteBatch)
{
	AnimatedEntity::Render(spriteBatch);
	//@Extra: draw font
	const wchar_t * output = m_text.c_str();
	Vector2 m_fontPos = Vector2(m_screenPos.x, m_screenPos.y - 50);
	Vector2 origin = m_font->MeasureString(output) / 2.f;
	
	m_font->DrawString(spriteBatch, output,
		m_fontPos , Colors::White, 0.f, origin);
}
