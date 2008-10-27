#ifndef H_BULLET_MANAGER
#define H_BULLET_MANAGER

#include "Entity.hpp"
#include "Weapon.hpp"

#include <list>
#include <SFML/Graphics.hpp>


/*
 * Classe singleton pour gérer les projectiles
 */
class BulletManager
{
public:
    
    /*
     * Récupérer l'instance
     */
    static BulletManager& GetInstance();
    
    /*
     * Déplacer les projectiles
     */
    void Update(float frametime);
    
    /*
     * Ajouter un projectile dans le gestionnaire
     */
    void Add(Weapon::Type type, const sf::Vector2f& offset, float angle);
    
    /*
     * Gestion des collisions
     */
    void Collide(Entity::ManagedContainer& entities);
    
    /*
     * Afficher les beams dans la fenêtre de rendu
     */
    void Show(sf::RenderWindow& app);
    
    /*
     * Suppression de tous les projectiles
     */
    void Clear();
    
private:
    struct Bullet
    {
        sf::Sprite sprite;
        float angle;
        int damage;
        int speed;
    };
    
    BulletManager();
    BulletManager(const BulletManager& other);
    BulletManager& operator=(const BulletManager& other);
    
    typedef std::list<Bullet>::iterator BulletIterator;
    
    std::list<Bullet> bullets_;
};

#endif /* guard H_BULLET_MANAGER */

