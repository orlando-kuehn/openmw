#ifndef _GAME_RENDER_ANIMATION_H
#define _GAME_RENDER_ANIMATION_H

#include <OgreController.h>
#include <OgreVector3.h>

#include <components/nifogre/ogrenifloader.hpp>

#include "../mwworld/ptr.hpp"


namespace MWRender
{

class Animation
{
protected:
    class AnimationValue : public Ogre::ControllerValue<Ogre::Real>
    {
    private:
        Animation *mAnimation;

    public:
        AnimationValue(Animation *anim)
          : mAnimation(anim)
        { }

        virtual Ogre::Real getValue() const;
        virtual void setValue(Ogre::Real value);
    };

    struct AnimSource {
        NifOgre::TextKeyMap mTextKeys;
        std::vector<Ogre::Controller<Ogre::Real> > mControllers;
    };
    typedef std::vector<AnimSource> AnimSourceList;

    struct AnimState {
        NifOgre::TextKeyMap::const_iterator mStartKey;
        NifOgre::TextKeyMap::const_iterator mLoopStartKey;
        NifOgre::TextKeyMap::const_iterator mStopKey;
        NifOgre::TextKeyMap::const_iterator mNextKey;

        float mTime;

        bool mPlaying;
        size_t mLoopCount;

        AnimState() : mTime(0.0f), mPlaying(false), mLoopCount(0)
        { }
    };
    typedef std::map<std::string,AnimState> AnimStateMap;

    MWWorld::Ptr mPtr;

    Ogre::SceneNode *mInsert;
    Ogre::Entity *mSkelBase;
    NifOgre::ObjectList mObjectRoot;
    AnimSourceList mAnimSources;
    Ogre::Node *mAccumRoot;
    Ogre::Node *mNonAccumRoot;
    NifOgre::NodeTargetValue<Ogre::Real> *mNonAccumCtrl;
    Ogre::Vector3 mAccumulate;
    Ogre::Vector3 mLastPosition;

    float mAnimVelocity;
    float mAnimSpeedMult;

    AnimStateMap mStates;

    // Note: One per animation group (lower body, upper body, left arm, etc).
    AnimSource *mSource;
    std::string mAnimationName;
    Ogre::SharedPtr<AnimationValue> mAnimationValuePtr;

    static float calcAnimVelocity(const NifOgre::TextKeyMap &keys,
                                  NifOgre::NodeTargetValue<Ogre::Real> *nonaccumctrl,
                                  const Ogre::Vector3 &accum,
                                  const std::string &groupname);

    /* Updates a skeleton instance so that all bones matching the source skeleton (based on
     * bone names) are positioned identically. */
    void updateSkeletonInstance(const Ogre::SkeletonInstance *skelsrc, Ogre::SkeletonInstance *skel);

    /* Updates the position of the accum root node for the given time, and
     * returns the wanted movement vector from the previous update. */
    void updatePosition(float time, Ogre::Vector3 &position);

    static NifOgre::TextKeyMap::const_iterator findGroupStart(const NifOgre::TextKeyMap &keys, const std::string &groupname);

    /* Resets the animation to the time of the specified start marker, without
     * moving anything, and set the end time to the specified stop marker. If
     * the marker is not found, or if the markers are the same, it returns
     * false.
     */
    bool reset(AnimState &state, const NifOgre::TextKeyMap &keys,
               NifOgre::NodeTargetValue<Ogre::Real> *nonaccumctrl,
               const std::string &groupname, const std::string &start, const std::string &stop,
               float startpoint);

    bool doLoop(AnimState &state);

    bool handleTextKey(AnimState &state, const std::string &groupname, const NifOgre::TextKeyMap::const_iterator &key);

    void setObjectRoot(Ogre::SceneNode *node, const std::string &model, bool baseonly);
    void addAnimSource(const std::string &model);

    static void destroyObjectList(Ogre::SceneManager *sceneMgr, NifOgre::ObjectList &objects);

    static void setRenderProperties(const NifOgre::ObjectList &objlist, Ogre::uint32 visflags, Ogre::uint8 solidqueue, Ogre::uint8 transqueue);

    void clearAnimSources();

public:
    Animation(const MWWorld::Ptr &ptr);
    virtual ~Animation();

    void updatePtr(const MWWorld::Ptr &ptr);

    bool hasAnimation(const std::string &anim);

    // Specifies the axis' to accumulate on. Non-accumulated axis will just
    // move visually, but not affect the actual movement. Each x/y/z value
    // should be on the scale of 0 to 1.
    void setAccumulation(const Ogre::Vector3 &accum);

    void setSpeed(float speed);

    /** Plays an animation.
     * \param groupname Name of the animation group to play.
     * \param start Key marker from which to start.
     * \param stop Key marker to stop at.
     * \param startpoint How far in between the two markers to start. 0 starts
     *                   at the start marker, 1 starts at the stop marker.
     * \param loops How many times to loop the animation. This will use the
     *              "loop start" and "loop stop" markers if they exist,
     *              otherwise it will use "start" and "stop".
     * \return Boolean specifying whether the animation will return movement
     *         for the character at all.
     */
    bool play(const std::string &groupname, const std::string &start, const std::string &stop, float startpoint, size_t loops);

    void disable(const std::string &groupname);

    /** Gets info about the given animation group.
     * \param groupname Animation group to check.
     * \param complete Stores completion amount (0 = at start key, 0.5 = half way between start and stop keys), etc.
     * \param start Stores the start key
     * \param stop Stores the stop key
     * \return True if the animation is active, false otherwise.
     */
    bool getInfo(const std::string &groupname, float *complete=NULL, std::string *start=NULL, std::string *stop=NULL) const;

    virtual Ogre::Vector3 runAnimation(float duration);

    virtual void showWeapons(bool showWeapon);

    Ogre::Node *getNode(const std::string &name);
};

}
#endif
