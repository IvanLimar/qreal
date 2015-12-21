/* Copyright 2007-2015 QReal Research Group
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

#include "trikRussianCGeneratorLibrary/trikRussianCGeneratorPluginBase.h"

#include <QtWidgets/QApplication>
#include <QtCore/QDir>
#include <QtCore/QProcess>

#include <qrkernel/settingsManager.h>
#include <utils/tcpRobotCommunicator.h>

#include "trikRussianCMasterGenerator.h"
#include "trikRussianCAdditionalPreferences.h"

using namespace trik::russianC;
using namespace qReal;

const QString robotModelName = "TrikRussianCGeneratorRobotModel";

TrikRussianCGeneratorPluginBase::TrikRussianCGeneratorPluginBase(
		kitBase::robotModel::RobotModelInterface * const robotModel
		, kitBase::blocksBase::BlocksFactoryInterface * const blocksFactory
		, const QStringList &pathsToTemplates)
	: TrikGeneratorPluginBase(robotModel, blocksFactory)
	, mGenerateCodeAction(new QAction(nullptr))
	, mUploadProgramAction(new QAction(nullptr))
	, mRunProgramAction(new QAction(nullptr))
	, mStopRobotAction(new QAction(nullptr))
	, mPathsToTemplates(pathsToTemplates)
{
	mGenerateCodeAction->setText(tr("Generate russian C code"));
	mGenerateCodeAction->setIcon(QIcon(":/russianC/images/generateRussianCCode.svg"));
	connect(mGenerateCodeAction, &QAction::triggered, this, &TrikRussianCGeneratorPluginBase::generateCode);

	mUploadProgramAction->setText(tr("Upload program russian C"));
	mUploadProgramAction->setIcon(QIcon(":/russianC/images/uploadProgram.svg"));
	connect(mUploadProgramAction, &QAction::triggered, this, &TrikRussianCGeneratorPluginBase::uploadProgram);

	mRunProgramAction->setText(tr("Run program russian C"));
	mRunProgramAction->setIcon(QIcon(":/russianC/images/run.png"));
	connect(mRunProgramAction, &QAction::triggered, this, &TrikRussianCGeneratorPluginBase::runProgram);

	mStopRobotAction->setText(tr("Stop robot"));
	mStopRobotAction->setIcon(QIcon(":/russianC/images/stop.png"));
	connect(mStopRobotAction, &QAction::triggered, this, &TrikRussianCGeneratorPluginBase::stopRobot);
}

TrikRussianCGeneratorPluginBase::~TrikRussianCGeneratorPluginBase()
{
	if (mOwnsAdditionalPreferences) {
		delete mAdditionalPreferences;
	}
}

QList<ActionInfo> TrikRussianCGeneratorPluginBase::customActions()
{
	const ActionInfo generateCodeActionInfo(mGenerateCodeAction, "generators", "tools");
	const ActionInfo uploadProgramActionInfo(mUploadProgramAction, "generators", "tools");
	const ActionInfo runProgramActionInfo(mRunProgramAction, "interpreters", "tools");
	const ActionInfo stopRobotActionInfo(mStopRobotAction, "interpreters", "tools");
	return {generateCodeActionInfo, uploadProgramActionInfo, runProgramActionInfo, stopRobotActionInfo};
}

QList<HotKeyActionInfo> TrikRussianCGeneratorPluginBase::hotKeyActions()
{
	mGenerateCodeAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
	mUploadProgramAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
	mRunProgramAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F6));
	mStopRobotAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_F6));

	HotKeyActionInfo generateCodeInfo("Generator.GenerateRussianC", tr("Generate russian C Code"), mGenerateCodeAction);
	HotKeyActionInfo uploadProgramInfo("Generator.UploadRussianC", tr("Upload russian C Program"), mUploadProgramAction);
	HotKeyActionInfo runProgramInfo("Generator.RunRussianC", tr("Run russian C Program"), mRunProgramAction);
	HotKeyActionInfo stopRobotInfo("Generator.StopRussianC", tr("Stop russian C Robot"), mStopRobotAction);

	return {generateCodeInfo, uploadProgramInfo, runProgramInfo, stopRobotInfo};
}

QIcon TrikRussianCGeneratorPluginBase::iconForFastSelector(
		const kitBase::robotModel::RobotModelInterface &robotModel) const
{
	Q_UNUSED(robotModel)
	return QIcon(":/russianC/images/switch-to-trik-russian-c.svg");
}

QList<kitBase::AdditionalPreferences *> TrikRussianCGeneratorPluginBase::settingsWidgets()
{
	mOwnsAdditionalPreferences = false;
	return {mAdditionalPreferences};
}

generatorBase::MasterGeneratorBase *TrikRussianCGeneratorPluginBase::masterGenerator()
{
	return new TrikRussianCMasterGenerator(*mRepo
			, *mMainWindowInterface->errorReporter()
			, *mParserErrorReporter
			, *mRobotModelManager
			, *mTextLanguage
			, mMainWindowInterface->activeDiagram()
			, mPathsToTemplates);
}

QString TrikRussianCGeneratorPluginBase::defaultFilePath(const QString &projectName) const
{
	return QString("trik/%1/%1.rc").arg(projectName);
}

text::LanguageInfo TrikRussianCGeneratorPluginBase::language() const
{
	//ReDo
	return qReal::text::Languages::russianC({ "robot" });
}

QString TrikRussianCGeneratorPluginBase::generatorName() const
{
	return "trikRussianC";
}

bool TrikRussianCGeneratorPluginBase::uploadProgram()
{
	QProcess compileProcess;
	const QFileInfo fileInfo = generateCodeForProcessing();
	const QFileInfo russianCCompiler = QFileInfo("russianCCompiler");
	compileProcess.setWorkingDirectory(russianCCompiler.absoluteDir().path());
	compileProcess.start(russianCCompiler.fileName(), {fileInfo.absoluteFilePath()});
	compileProcess.waitForFinished();

	const QFileInfo winSCP = QFileInfo(qReal::SettingsManager::value("WinScpPath").toString());

	if (!winSCP.exists()){
		mMainWindowInterface->errorReporter()->addError(
			tr("Please provide path to the WinSCP in Settings dialog.")
		);

		return false;
	}

	const QFileInfo exportInfo = QFileInfo("export.txt");

	const QString moveCommand = QString(
			"\"%1\" /command  \"open scp://root@%2\" "
			"\"call mkdir -p /home/root/trik/RussianC/Environment/\" "
			"\"put %3 /home/root/trik/RussianC/Environment/\"")
			.arg(qReal::SettingsManager::value("WinScpPath").toString())
			.arg(qReal::SettingsManager::value("TrikTcpServer").toString())
			.arg(exportInfo.absoluteFilePath().replace("/","\\"));

	QProcess deployProcess;
	if (!deployProcess.startDetached(moveCommand)) {
		mMainWindowInterface->errorReporter()->addError(tr("Unable to launch WinSCP"));
		return false;
	}

	mMainWindowInterface->errorReporter()->addInformation(
		tr("After downloading the program, enter 'exit' or close the window")
	);

	return true;
}

void TrikRussianCGeneratorPluginBase::runProgram()
{
	const QString runCommand = QString(
			"\"%1\" /command  \"open scp://root@%2\" "
			"\"put run.sh /home/root/trik/RussianC/Environment/\" "
			"\"call home/root/trik/RussianC/Environment/run.sh\" ")
			.arg(qReal::SettingsManager::value("WinScpPath").toString())
			.arg(qReal::SettingsManager::value("TrikTcpServer").toString());
	/*const QString runCommand = QString(
				"\"%1\" /command  \"open scp://root@%2\" "
				"\"call home/root/trik/RussianC/Environment/RussianCInterprerer\" ")
				.arg(qReal::SettingsManager::value("WinScpPath").toString())
				.arg(qReal::SettingsManager::value("TrikTcpServer").toString());*/
	QProcess runProcess;
	if(!runProcess.startDetached(runCommand))
	{
		mMainWindowInterface->errorReporter()->addError(tr("Please download to robot russian C interpreter."));
	}
}

void TrikRussianCGeneratorPluginBase::stopRobot()
{
	//ToDo
}
