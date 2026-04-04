// Created by Joey Driedger

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ROOMMANAGER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ROOMMANAGER_H

#include <vector>

namespace engine
{
    class RoomManager
    {
    public:
        void AddRoom(int roomId);
        [[nodiscard]] bool HasRoom(int roomId) const;

        void SetCurrentRoom(int roomId);
        [[nodiscard]] int GetCurrentRoom() const;

        void SetTransitionTarget(int roomId);
        [[nodiscard]] bool HasTransitionTarget() const;
        [[nodiscard]] int GetTransitionTarget() const;

        void ClearTransitionTarget();

    private:
        std::vector<int> m_Rooms;
        int m_CurrentRoomId = -1;
        int m_TransitionTarget = -1;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_ROOMMANAGER_H