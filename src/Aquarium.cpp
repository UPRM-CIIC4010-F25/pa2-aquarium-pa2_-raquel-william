#include "Aquarium.h"
#include <cstdlib>


string AquariumCreatureTypeToString(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return "BiggerFish";
        case AquariumCreatureType::NPCreature:
            return "BaseFish";
        case AquariumCreatureType::PinkFish:
            return "PinkFish";
        case AquariumCreatureType::SharkFish:
            return "SharkFish";
        default:
            return "UknownFish";
    }
}

//Power Up Implementation
PowerUp::PowerUp(float x, float y, float r, std::shared_ptr<GameSprite> sprite)
    : m_x(x), m_y(y), m_radius(r), m_sprite(std::move(sprite)) {}

float PowerUp::getX() const { return m_x; }
float PowerUp::getY() const { return m_y; }
float PowerUp::getRadius() const { return m_radius; }

void PowerUp::draw() const {
    if (m_sprite) m_sprite->draw(m_x, m_y); //draws starting from top-left
};

// PlayerCreature Implementation
PlayerCreature::PlayerCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 10.0f, 1, sprite) {}


void PlayerCreature::setDirection(float dx, float dy) {
    m_dx = dx;
    m_dy = dy;
    normalize();
}

void PlayerCreature::move() {
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    this->bounce();
}

void PlayerCreature::reduceDamageDebounce() {
    if (m_damage_debounce > 0) {
        --m_damage_debounce;
    }
}

void PlayerCreature::update() {
    this->reduceDamageDebounce();
    this->move();
}


void PlayerCreature::draw() const {
    
    ofLogVerbose() << "PlayerCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    if (this->m_damage_debounce > 0) {
        ofSetColor(ofColor::red); // Flash red if in damage debounce
    }
    if (m_sprite) { //incr size after pu
       ofPushMatrix();
        ofTranslate(m_x, m_y);
        ofScale(m_visualScale, m_visualScale); 
        m_sprite->draw(0, 0);
        ofPopMatrix();
    }
    ofSetColor(ofColor::white); // Reset color

}

void PlayerCreature::changeSpeed(int speed) {
    m_speed = speed;
}

void PlayerCreature::loseLife(int debounce) {
    if (m_damage_debounce <= 0) {
        if (m_lives > 0) this->m_lives -= 1;
        m_damage_debounce = debounce; // Set debounce frames
        ofLogNotice() << "Player lost a life! Lives remaining: " << m_lives << std::endl;
    }
    // If in debounce period, do nothing
    if (m_damage_debounce > 0) {
        ofLogVerbose() << "Player is in damage debounce period. Frames left: " << m_damage_debounce << std::endl;
    }
}

//growth func
void PlayerCreature::setPermanentSize(float scaleUp) {
    m_visualScale = scaleUp;

    setCollisionRadius(getCollisionRadius() * scaleUp);
}

// NPCreature Implementation
NPCreature::NPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 30, 1, sprite) {
    m_dx = (rand() % 3 - 1); // -1, 0, or 1
    m_dy = (rand() % 3 - 1); // -1, 0, or 1
    normalize();

    m_creatureType = AquariumCreatureType::NPCreature;
}

void NPCreature::move() {
    // Simple AI movement logic (random direction)
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }
    bounce();
}

void NPCreature::draw() const {
    ofLogVerbose() << "NPCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    ofSetColor(ofColor::white);
    if (m_sprite) {
        m_sprite->draw(m_x, m_y);
    }
}


BiggerFish::BiggerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {
    m_dx = (rand() % 3 - 1);
    m_dy = (rand() % 3 - 1);
    normalize();

    setCollisionRadius(60); // Bigger fish have a larger collision radius
    m_value = 5; // Bigger fish have a higher value
    m_creatureType = AquariumCreatureType::BiggerFish;
}

void BiggerFish::move() {
    // Bigger fish might move slower or have different logic
    m_x += m_dx * (m_speed * 0.5); // Moves at half speed
    m_y += m_dy * (m_speed * 0.5);
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }

    bounce();
}

void BiggerFish::draw() const {
    ofLogVerbose() << "BiggerFish at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    this->m_sprite->draw(this->m_x, this->m_y);
}

PinkFish::PinkFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {
    m_dx = 1;
    m_dy = 0;
    normalize();

    setCollisionRadius(30);
    m_value = 2;
    m_creatureType = AquariumCreatureType::PinkFish;
}  

void PinkFish::move(){
    t += 0.05f;
    float amplitude = 2.0f;
    float sinY = sinf(t) * amplitude;

    m_x += m_dx * m_speed;
    m_y += (m_dy + sinY) * 0.5f * m_speed;

    if (m_dx < 0) this->m_sprite->setFlipped(true);
    else          this->m_sprite->setFlipped(false);

    bounce();
}

void PinkFish::draw() const{
    ofLogVerbose() << "PinkFish at (" << m_x << ", " << m_y << ") speed: " << m_speed << std::endl;
    if (m_sprite) m_sprite->draw(m_x, m_y);
}

SharkFish::SharkFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {

    m_dx = (rand() % 2 == 0) ? 1 : -1;
    m_dy = 0;
    normalize();

    setCollisionRadius(45);               
    m_value = 10;                         
    m_creatureType = AquariumCreatureType::SharkFish;

    dashFrames = 0;
    cooldownFrames = 60 + rand() % 120;   
}

void SharkFish::move() {
    m_sprite->setFlipped(m_dx < 0);

    float speedMul = 1.4f;     
    if (dashFrames > 0) {
        speedMul = 2.6f;       
        dashFrames--;
    } else {
        //count down till next dash
        if (cooldownFrames > 0) cooldownFrames--;
        else {
           //rand dash
            if ((rand() % 100) < 12) {      
                dashFrames = 18;           
                cooldownFrames = 90 + rand() % 120; 
            }
        }
        
        m_dy += (rand() % 3 - 1) * 0.02f;  
        if (m_dy >  0.6f) m_dy =  0.6f;
        if (m_dy < -0.6f) m_dy = -0.6f;
        normalize();
    }

    m_x += m_dx * (m_speed * speedMul);
    m_y += m_dy * (m_speed * speedMul);

    bounce(); 
}

void SharkFish::draw() const {
    if (m_sprite) m_sprite->draw(m_x, m_y);
}

// AquariumSpriteManager
AquariumSpriteManager::AquariumSpriteManager(){
    this->m_npc_fish = std::make_shared<GameSprite>("base-fish.png", 70,70);
    this->m_big_fish = std::make_shared<GameSprite>("bigger-fish.png", 120, 120);
    this->m_pink_fish = std::make_shared<GameSprite>("pinkFish.png", 80, 80);
    this->m_shark_fish = std::make_shared<GameSprite>("sharkFish.png", 100, 100);
}

std::shared_ptr<GameSprite> AquariumSpriteManager::GetSprite(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return std::make_shared<GameSprite>(*this->m_big_fish);
            
        case AquariumCreatureType::NPCreature:
            return std::make_shared<GameSprite>(*this->m_npc_fish);

        case AquariumCreatureType::PinkFish:
            return std::make_shared<GameSprite>(*this->m_pink_fish);

        case AquariumCreatureType::SharkFish:
            return std::make_shared<GameSprite>(*this->m_shark_fish);

        default:
            return nullptr;
    }
}


// Aquarium Implementation
Aquarium::Aquarium(int width, int height, std::shared_ptr<AquariumSpriteManager> spriteManager)
    : m_width(width), m_height(height) {
        m_sprite_manager =  spriteManager;
    }



void Aquarium::addCreature(std::shared_ptr<Creature> creature) {
    creature->setBounds(m_width - 20, m_height - 20);
    m_creatures.push_back(creature);
}

void Aquarium::addAquariumLevel(std::shared_ptr<AquariumLevel> level){
    if(level == nullptr){return;} // guard to not add noise
    this->m_aquariumlevels.push_back(level);
}

void Aquarium::update() {
    for (auto& creature : m_creatures) {
        creature->move();
    }
    this->Repopulate();
}

void Aquarium::draw() const {
    for (const auto& creature : m_creatures) {
        creature->draw();
    }
    for (const auto& pu : m_powerups) {
    if (pu) pu->draw(); //power Ups drawn AFTER CREATURE!!!!!
}
}


void Aquarium::removeCreature(std::shared_ptr<Creature> creature) {
    auto it = std::find(m_creatures.begin(), m_creatures.end(), creature);
    if (it != m_creatures.end()) {
        ofLogVerbose() << "removing creature " << endl;
        int selectLvl = this->currentLevel % this->m_aquariumlevels.size();
        auto npcCreature = std::static_pointer_cast<NPCreature>(creature);
        this->m_aquariumlevels.at(selectLvl)->ConsumePopulation(npcCreature->GetType(), npcCreature->getValue());
        m_creatures.erase(it);
    }
}

void Aquarium::clearCreatures() {
    m_creatures.clear();
}

std::shared_ptr<Creature> Aquarium::getCreatureAt(int index) {
    if (index < 0 || size_t(index) >= m_creatures.size()) {
        return nullptr;
    }
    return m_creatures[index];
}



void Aquarium::SpawnCreature(AquariumCreatureType type) {
    int x = rand() % this->getWidth();
    int y = rand() % this->getHeight();
    int speed = 1 + rand() % 25; // Speed between 1 and 25

    switch (type) {
        case AquariumCreatureType::NPCreature:
            this->addCreature(std::make_shared<NPCreature>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::NPCreature)));
            break;
        case AquariumCreatureType::BiggerFish:
            this->addCreature(std::make_shared<BiggerFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::BiggerFish)));
            break;
        case AquariumCreatureType::PinkFish:
            this->addCreature(std::make_shared<PinkFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::PinkFish)));
            break;
        case AquariumCreatureType::SharkFish:
            this->addCreature(std::make_shared<SharkFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::SharkFish)));
            break;
        default:
            ofLogError() << "Unknown creature type to spawn!";
            break;
    }

}


// repopulation will be called from the levl class
// it will compose into aquarium so eating eats frm the pool of NPCs in the lvl class
// once lvl criteria met, we move to new lvl through inner signal asking for new lvl
// which will mean incrementing the buffer and pointing to a new lvl index
void Aquarium::Repopulate() {
    ofLogVerbose("entering phase repopulation");
    // lets make the levels circular
    int selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
    ofLogVerbose() << "the current index: " << selectedLevelIdx << endl;
    std::shared_ptr<AquariumLevel> level = this->m_aquariumlevels.at(selectedLevelIdx);


    if(level->isCompleted()){
        level->levelReset();
        this->currentLevel += 1;
        selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
        ofLogNotice()<<"new level reached : " << selectedLevelIdx << std::endl;
        level = this->m_aquariumlevels.at(selectedLevelIdx);
        this->clearCreatures();
    }

    
    // now lets find how many to respawn if needed 
    std::vector<AquariumCreatureType> toRespawn = level->Repopulate();
    ofLogVerbose() << "amount to repopulate : " << toRespawn.size() << endl;
    if(toRespawn.size() <= 0 ){return;} // there is nothing for me to do here
    for(AquariumCreatureType newCreatureType : toRespawn){
        this->SpawnCreature(newCreatureType);
    }
}


// Aquarium collision detection
std::shared_ptr<GameEvent> DetectAquariumCollisions(std::shared_ptr<Aquarium> aquarium, std::shared_ptr<PlayerCreature> player) {
    if (!aquarium || !player) return nullptr;
    
    for (int i = 0; i < aquarium->getCreatureCount(); ++i) {
        std::shared_ptr<Creature> npc = aquarium->getCreatureAt(i);
        if (npc && checkCollision(player, npc)) {
            return std::make_shared<GameEvent>(GameEventType::COLLISION, player, npc);
        }
    }
    return nullptr;
}

// power up methods inside aquarium
void Aquarium::addPowerUp(std::shared_ptr<PowerUp> pu) {
    m_powerups.push_back(std::move(pu));
}

int Aquarium::getPowerUpCount() const {
    return (int)m_powerups.size();
}

std::shared_ptr<PowerUp> Aquarium::getPowerUpAt(int i) {
    if (i < 0 || (size_t)i >= m_powerups.size()) return nullptr;
    return m_powerups[i];
}

void Aquarium::removePowerUp(const std::shared_ptr<PowerUp>& pu) {
    auto it = std::find(m_powerups.begin(), m_powerups.end(), pu);
    if (it != m_powerups.end()) m_powerups.erase(it);
};

//  Imlementation of the AquariumScene

void AquariumGameScene::Update(){
    std::shared_ptr<GameEvent> event;

    this->m_player->update();

    //detect if big fish was seen indicating level 2 start 
    if (!seenBigFish) {

    for (int i = 0; i < m_aquarium->getCreatureCount(); ++i) {
        auto c = m_aquarium->getCreatureAt(i);
        if (!c) continue;
        
        auto npc = std::dynamic_pointer_cast<NPCreature>(c);
        if (npc && npc->GetType() == AquariumCreatureType::BiggerFish) {
            seenBigFish = true;            
            framesSinceBigFishSeen = 0;    
            break;
        }
    }
    }
    // if big fish was seen then lvl then start timer for pu spawn
    if (seenBigFish && !spawnedSizePU) {
    framesSinceBigFishSeen++;
    if (framesSinceBigFishSeen > 10 * 60) { 
        auto spritePU = std::make_shared<GameSprite>("PowerUp.png", 32, 32);

        float px = m_player->getX(), py = m_player->getY();
        const float margin = 20.0f;
        float x = std::clamp(px + 150.0f, margin, float(m_aquarium->getWidth()  - margin));
        float y = std::clamp(py + 100.0f, margin, float(m_aquarium->getHeight() - margin));

        m_aquarium->addPowerUp(std::make_shared<PowerUp>(x, y, 16.0f, spritePU));
        spawnedSizePU = true;
        ofLogNotice() << "Power UP spawned 10s into Level 2";
    }
    }

    if (this->updateControl.tick()) {
        event = DetectAquariumCollisions(this->m_aquarium, this->m_player);
        if (event != nullptr && event->isCollisionEvent()) {
            ofLogVerbose() << "Collision detected between player and NPC!" << std::endl;
            if(event->creatureB != nullptr){
                event->print();
                if(this->m_player->getPower() < event->creatureB->getValue()){
                    ofLogNotice() << "Player is too weak to eat the creature!" << std::endl;
                    this->m_player->loseLife(3*60); // 3 frames debounce, 3 seconds at 60fps
                    if(this->m_player->getLives() <= 0){
                        this->m_lastEvent = std::make_shared<GameEvent>(GameEventType::GAME_OVER, this->m_player, nullptr);
                        return;
                    }
                }
                else{
                    this->m_aquarium->removeCreature(event->creatureB);
                    this->m_player->addToScore(1, event->creatureB->getValue());
                    if (this->m_player->getScore() % 25 == 0){
                        this->m_player->increasePower(1);
                        ofLogNotice() << "Player power increased to " << this->m_player->getPower() << "!" << std::endl;
                    }
                    
                }
                
                

            } else {
                ofLogError() << "Error: creatureB is null in collision event." << std::endl;
            }
        }

        // if collision with powerup is true then increase size and hitbox
        for (int i = 0; i < this->m_aquarium->getPowerUpCount(); i++) {
            auto pu = this->m_aquarium->getPowerUpAt(i);
            if (!pu) continue;

            float px = this->m_player->getX() + this->m_player->getCollisionRadius();
            float py = this->m_player->getY() + this->m_player->getCollisionRadius();
            float qx = pu->getX() + pu->getRadius();
            float qy = pu->getY() + pu->getRadius();

            float dx = px - qx;
            float dy = py - qy;
            float rr = this->m_player->getCollisionRadius() + pu->getRadius();

            if (dx*dx + dy*dy <= rr*rr) {
                m_player->setPermanentSize(1.5f); 

                ofLogNotice() << "PowerUp collected! New collision radius -> "
                             << m_player->getCollisionRadius();

                m_aquarium->removePowerUp(pu);
                break;
            }
        }

        this->m_aquarium->update();
    }

}

void AquariumGameScene::Draw() {
    this->m_player->draw();
    this->m_aquarium->draw();
    this->paintAquariumHUD();

}


void AquariumGameScene::paintAquariumHUD(){
    float panelWidth = ofGetWindowWidth() - 150;
    ofDrawBitmapString("Score: " + std::to_string(this->m_player->getScore()), panelWidth, 20);
    ofDrawBitmapString("Power: " + std::to_string(this->m_player->getPower()), panelWidth, 30);
    ofDrawBitmapString("Lives: " + std::to_string(this->m_player->getLives()), panelWidth, 40);
    for (int i = 0; i < this->m_player->getLives(); ++i) {
        ofSetColor(ofColor::red);
        ofDrawCircle(panelWidth + i * 20, 50, 5);
    }
    ofSetColor(ofColor::white); // Reset color to white for other drawings
}

void AquariumLevel::populationReset(){
    for(auto node: this->m_levelPopulation){
        node->currentPopulation = 0; // need to reset the population to ensure they are made a new in the next level
    }
}

void AquariumLevel::ConsumePopulation(AquariumCreatureType creatureType, int power){
    for(std::shared_ptr<AquariumLevelPopulationNode> node: this->m_levelPopulation){
        ofLogVerbose() << "consuming from this level creatures" << endl;
        if(node->creatureType == creatureType){
            ofLogVerbose() << "-cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            if(node->currentPopulation == 0){
                return;
            } 
            node->currentPopulation -= 1;
            ofLogVerbose() << "+cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            this->m_level_score += power;
            return;
        }
    }
}

bool AquariumLevel::isCompleted(){
    return this->m_level_score >= this->m_targetScore;
}




std::vector<AquariumCreatureType> Level_0::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        ofLogVerbose() << "to Repopulate :  " << delta << endl;
        if(delta >0){
            for(int i = 0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;

}

std::vector<AquariumCreatureType> Level_1::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        if(delta >0){
            for(int i=0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;
}

std::vector<AquariumCreatureType> Level_2::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        if(delta >0){
            for(int i=0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;
}
