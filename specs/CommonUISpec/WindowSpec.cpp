/**
 * Copyright (C) 2012-2014 Phonations
 * License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher
 */

#include "bandit/bandit.h"

#include <QAction>
#include <QTest>

#include "PhTools/PhDebug.h"

#include "WindowSpecSettings.h"
#include "WindowSpecWindow.h"

using namespace bandit;

go_bandit([](){
	describe("window_test", [](){

		before_each([&](){
			PhDebug::disable();
		});

		it("go_fullscreen_01", [&](){
			WindowSpecSettings settings(true);
			QByteArray emptyArray;
			settings.setWindowGeometry(emptyArray);
			settings.setFullScreen(false);
			WindowSpecWindow w(&settings);
			w.show();
			AssertThat(w.isFullScreen(), IsFalse());
			AssertThat(w.fullScreenAction()->isChecked(), IsFalse());

			QTest::qWait(1000);

			w.toggleFullScreen();
			AssertThat(w.isFullScreen(), IsTrue());
			AssertThat(settings.fullScreen(), IsTrue());
			AssertThat(w.fullScreenAction()->isChecked(), IsTrue());

			QTest::qWait(2000);

			w.toggleFullScreen();

			AssertThat(w.isFullScreen(), IsFalse());
			AssertThat(settings.fullScreen(), IsFalse());
			AssertThat(w.fullScreenAction()->isChecked(), IsFalse());

			QTest::qWait(1000);

			w.close();
		});

		it("go_fullscreen_02", [&](){
			WindowSpecSettings settings(true);

			settings.setFullScreen(true);
			WindowSpecWindow w(&settings);
			w.show();
			QTest::qWait(3000);
			AssertThat(w.isFullScreen(), IsTrue());
			AssertThat(w.fullScreenAction()->isChecked(), IsTrue());

			w.fullScreenAction()->trigger();

			QTest::qWait(1000);
			AssertThat(w.isFullScreen(), IsFalse());
			AssertThat(w.fullScreenAction()->isChecked(), IsFalse());

			w.close();
		});
	});
});
