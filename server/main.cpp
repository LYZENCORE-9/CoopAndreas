#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>

#include "thirdparty-libraries/enet/enet.h"

#include "core/CControllerState.h"
#include "core/NetworkEntityType.h"
#include "core/CPacketListener.h"
#include "core/CVector.h"
#include "core/CNetwork.h"
#include "core/CPed.h"
#include "core/CPedManager.h"
#include "core/CPlayer.h"
#include "core/CPlayerManager.h"
#include "core/CVehicle.h"
#include "core/CVehicleManager.h"
#include "core/VehicleDoorState.h"

#include "core_external/ConfigDatabase.hpp"



int main(int argc, char *argv[])
{
	ConfigDatabase::Init("server-config.ini");
	CNetwork::Init(ConfigDatabase::configport);
	return 0;
}