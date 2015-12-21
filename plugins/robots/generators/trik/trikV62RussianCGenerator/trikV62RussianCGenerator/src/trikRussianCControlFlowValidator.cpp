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

#include "trikRussianCControlFlowValidator.h"

using namespace generatorBase;
using namespace trik::russianC;

TrikRussianCControlFlowValidator::TrikRussianCControlFlowValidator(const qrRepo::RepoApi &repo
		, qReal::ErrorReporterInterface &errorReporter
		, GeneratorCustomizer &customizer
		, QObject *parent)
	: PrimaryControlFlowValidator(repo, errorReporter, customizer, parent)
{
}

PrimaryControlFlowValidator *TrikRussianCControlFlowValidator::clone()
{
	return new TrikRussianCControlFlowValidator(mRepo, mErrorReporter, mCustomizer, parent());
}

void TrikRussianCControlFlowValidator::visitRegular(const qReal::Id &id, const QList<LinkInfo> &links)
{
	Q_UNUSED(links)
	if (id.element() == "SendMessageThreads" || id.element() == "ReceiveMessageThreads"
			|| id.element() == "KillThread") {
		error(tr("Block type is unsupported by russian C generator"), id);
	}

	PrimaryControlFlowValidator::visitRegular(id, links);
}

void TrikRussianCControlFlowValidator::visitJoin(const qReal::Id &id, QList<LinkInfo> &links)
{
	Q_UNUSED(links)
	error(tr("Block type is unsupported by russian C"), id);
	PrimaryControlFlowValidator::visitJoin(id, links);
}
