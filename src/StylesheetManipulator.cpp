/**
MIT License

Copyright (c) 2018 Michael Scopchanov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "StylesheetManipulator.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>

QString StylesheetManipulator::updateStylesheetProperty(const QString &styleSheet, const QString &selector, const QString &property, const QString &newValue)
{
	return jsonToStyleSheet(updateValue(selector, property, newValue, styleSheetToJson(styleSheet)));
}

QJsonArray StylesheetManipulator::styleSheetToJson(const QString &styleSheet)
{
	QJsonArray jsonStyleSheet;

	if (styleSheet.isEmpty())
		return jsonStyleSheet;

	foreach (const QString &style, styleSheet.trimmed().split("}")) {
		const QString &trimmedStyle(style.trimmed());

		if (!trimmedStyle.isEmpty()) {
			const QStringList &list(trimmedStyle.split("{"));

			jsonStyleSheet.append(QJsonObject {
								 {"selector", list.first().trimmed()},
								 {"properties", styleSheetPropertiesToJson(list.last())}
							 });
		}
	}

	return jsonStyleSheet;
}

QJsonArray StylesheetManipulator::styleSheetPropertiesToJson(const QString &properties)
{
	QJsonArray jsonProperties;

	if (properties.isEmpty())
		return jsonProperties;

	foreach (const QString &property, properties.trimmed().split(";")) {
		const QString &trimmedProperty(property.trimmed());

		if (!trimmedProperty.isEmpty()) {
			const QStringList &list(trimmedProperty.split(":"));

			jsonProperties.append(QJsonObject{
									  {"name", list.first().trimmed()},
									  {"value", list.last().trimmed()}
								  });
		}
	}

	return jsonProperties;
}

QJsonArray StylesheetManipulator::updateValue(const QString &selector, const QString &propertyName, const QString &newValue, const QJsonArray &jsonStyleSheet)
{
	QJsonArray a;

	foreach (const QJsonValue &value, jsonStyleSheet) {
		const QJsonObject &currentStyle(value.toObject());
		const QString &currentSelector(currentStyle["selector"].toString());
		bool selectorFound = currentSelector == selector;
		QJsonArray properties;

		foreach (const QJsonValue &value, currentStyle["properties"].toArray()) {
			QJsonObject property(value.toObject());

			if (selectorFound && (property["name"].toString() == propertyName))
				property["value"] = newValue;

			properties.append(property);
		}

		a.append(QJsonObject{
					 {"selector", currentSelector},
					 {"properties", properties}
				 });
	}

	return a;
}

QString StylesheetManipulator::jsonToStyleSheet(const QJsonArray &jsonStyleSheet)
{
	QString styleSheet;

	foreach (const QJsonValue &value, jsonStyleSheet) {
		const QJsonObject &currentStyle(value.toObject());

		styleSheet.append(currentStyle["selector"].toString() + " {");

		foreach (const QJsonValue &value, currentStyle["properties"].toArray()) {
			QJsonObject property(value.toObject());

			styleSheet.append(" " + property["name"].toString() + ": " + property["value"].toString() + ";");
		}

		styleSheet.append(" } ");
	}

	return styleSheet;
}
