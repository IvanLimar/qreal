/* Copyright 2016 CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#pragma once

#include <kitBase/robotModel/robotParts/touchSensor.h>
#include <utils/robotCommunication/tcpRobotCommunicator.h>

#include "trikKitInterpreterCommon/declSpec.h"

namespace trik {
namespace robotModel {
namespace real {
namespace parts {

class ROBOTS_TRIK_KIT_INTERPRETER_COMMON_EXPORT TouchSensor : public kitBase::robotModel::robotParts::TouchSensor
{
	Q_OBJECT

public:
	TouchSensor(const kitBase::robotModel::DeviceInfo &info
			, const kitBase::robotModel::PortInfo &port
			, utils::robotCommunication::TcpRobotCommunicator &tcpRobotCommunicator);

	void read() override;

public slots:
	void onIncomingData(const QString &portName, int value);

private:
	utils::robotCommunication::TcpRobotCommunicator &mRobotCommunicator;
};

}
}
}
}
