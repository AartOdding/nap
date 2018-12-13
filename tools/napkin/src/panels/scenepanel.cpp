#include "scenepanel.h"
#include <appcontext.h>
#include <sceneservice.h>
#include <standarditemsobject.h>
#include <commands.h>
#include <napqt/filterpopup.h>
#include <naputils.h>


/**
 * @return All currently loaded scenes
 */
nap::SceneService::SceneSet getScenes()
{
	return napkin::AppContext::get().getCore().getService<nap::SceneService>()->getScenes();
}


napkin::SceneModel::SceneModel() : QStandardItemModel()
{
    setHorizontalHeaderLabels({"Name"});

    connect(&AppContext::get(), &AppContext::documentOpened, this, &SceneModel::onFileOpened);
    connect(&AppContext::get(), &AppContext::newDocumentCreated, this, &SceneModel::onNewFile);
	connect(&AppContext::get(), &AppContext::objectAdded, this, &SceneModel::onObjectAdded);
	connect(&AppContext::get(), &AppContext::objectChanged, this, &SceneModel::onObjectChanged);
}

void napkin::SceneModel::refresh()
{
    while (rowCount() > 0)
        removeRow(0);

	for (auto scene : getScenes())
		appendRow(new SceneItem(*scene));

}

void napkin::SceneModel::onObjectAdded(nap::rtti::Object* obj)
{
	// TODO: Don't refresh entire model
	refresh();
}

void napkin::SceneModel::onObjectRemoved(nap::rtti::Object* obj)
{
	// TODO: Don't refresh entire model
	refresh();
}

void napkin::SceneModel::onObjectChanged(nap::rtti::Object* obj)
{
	// TODO: Don't refresh entire model
	refresh();
}

void napkin::SceneModel::onNewFile()
{
    refresh();
}

void napkin::SceneModel::onFileOpened(const QString& filename)
{
    refresh();
}

napkin::ScenePanel::ScenePanel() : QWidget()
{
	setLayout(&mLayout);
	mLayout.setContentsMargins(0, 0, 0, 0);
	layout()->addWidget(&mFilterView);
    mFilterView.setModel(&mModel);
	mFilterView.setMenuHook(std::bind(&napkin::ScenePanel::menuHook, this, std::placeholders::_1));
	mFilterView.getTreeView().expandAll();

	connect(&mModel, &QAbstractItemModel::rowsInserted, [this](const QModelIndex &parent, int first, int last) {
		mFilterView.getTreeView().expandAll();
	});
}

void napkin::ScenePanel::menuHook(QMenu& menu)
{
	auto item = mFilterView.getSelectedItem();

	{
		auto sceneItem = dynamic_cast<SceneItem*>(item);
		if (sceneItem)
		{
			auto scene = rtti_cast<nap::Scene>(sceneItem->getObject());
			assert(scene->get_type().is_derived_from<nap::Scene>());

			auto addEntityAction = menu.addAction("Add Entity...");
			connect(addEntityAction, &QAction::triggered, [this, sceneItem, scene]()
			{
				auto entity = napkin::showObjectSelector<nap::Entity>(this);
				if (!entity)
					return;

				AppContext::get().executeCommand(new AddEntityToSceneCommand(*scene, *entity));
			});


		}
	}

	auto entityInstanceItem = dynamic_cast<EntityInstanceItem*>(item);
	if (entityInstanceItem)
	{
		auto sceneItem = dynamic_cast<SceneItem*>(entityInstanceItem->parent());

		if (sceneItem)
		{
			auto scene = rtti_cast<nap::Scene>(sceneItem->getObject());
			assert(scene);
			auto entity = rtti_cast<nap::Entity>(entityInstanceItem->getObject());
			assert(entity);

			auto removeEntityAction = menu.addAction("Delete Instance");
			connect(removeEntityAction, &QAction::triggered, [this, scene, entity]
			{
				AppContext::get().executeCommand(new RemoveEntityFromSceneCommand(*scene, *entity));
			});
		}
	}

//	menu.addAction("Add Scene", []()
//	{
//		AppContext::get().executeCommand(new AddObjectCommand(RTTI_OF(nap::Scene)));
//	});
}