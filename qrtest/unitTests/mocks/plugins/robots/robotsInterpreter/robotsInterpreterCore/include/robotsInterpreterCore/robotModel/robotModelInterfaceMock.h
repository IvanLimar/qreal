#pragma once

#include <robotsInterpreterCore/robotModel/robotModelInterface.h>

#include <gmock/gmock.h>

namespace qrTest {

class RobotModelInterfaceMock : public robotsInterpreterCore::robotModel::RobotModelInterface
{
public:
	MOCK_METHOD0(init, void());
	MOCK_METHOD0(stopRobot, void());
	MOCK_METHOD0(disconnectFromRobot, void());
	MOCK_CONST_METHOD0(needsConnection, bool());
};

}